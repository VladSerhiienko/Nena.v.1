using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Shapes;

namespace CSWPFApp.Physics
{
    class VectorTag3
    {
        public float X, Y, Z;
    }

    class ProjectileSimulatorConfigItem
    {
        public string Key { get; set; }
        public float Value { get; set; }
    }

    class ProjectileSimulatorConfig
    {
        public ObservableCollection<ProjectileSimulatorConfigItem> Items { get; set; }

        public ProjectileSimulatorConfig()
        {
            Items = new ObservableCollection<ProjectileSimulatorConfigItem>();

            Items.Add(new ProjectileSimulatorConfigItem { Key = "Sim.L", Value = 65.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Sim.Vm", Value = 50.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Sim.Yb", Value = 10.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Sim.Alpha", Value = 25.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Sim.Gamma", Value = 8.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Sim.TimeDelta", Value = 0.05f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Sim.TimeTicks", Value = 3600.0f });

            Items.Add(new ProjectileSimulatorConfigItem { Key = "Tgt.X", Value = 400.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Tgt.Y", Value = 75.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Tgt.Z", Value = 45.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Tgt.W", Value = 60.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Tgt.H", Value = 50.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Tgt.L", Value = 80.0f });

            Items.Add(new ProjectileSimulatorConfigItem { Key = "Trn.W", Value = 200.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Trn.H", Value = 100.0f });
            Items.Add(new ProjectileSimulatorConfigItem { Key = "Trn.L", Value = 800.0f });
        }
    }

    class Projectile
    {
        public float Sx, Sy, Sz;
        public float Vx, Vy, Vz;
    }

    class Terraine
    {
        public float W, H, L;

        public void ReadParameters<T>(T parameters) where T : IEnumerable<Physics.ProjectileSimulatorConfigItem>
        {
            foreach (var parameter in parameters)
            {
                if (parameter.Key == "Trn.L") L = parameter.Value; else
                if (parameter.Key == "Trn.W") W = parameter.Value; else
                if (parameter.Key == "Trn.H") H = parameter.Value;
            }
        }
    }

    class Target
    {
        public float X, Y, Z;
        public float L, W, H;

        public Target() 
        {
        }

        public void ReadParameters<T>(T parameters) where T : IEnumerable<Physics.ProjectileSimulatorConfigItem>
        {
            foreach (var parameter in parameters)
            {
                if (parameter.Key == "Tgt.X") X = parameter.Value; else
                if (parameter.Key == "Tgt.Y") Y = parameter.Value; else
                if (parameter.Key == "Tgt.Z") Z = parameter.Value; else
                if (parameter.Key == "Tgt.L") L = parameter.Value; else
                if (parameter.Key == "Tgt.W") W = parameter.Value; else
                if (parameter.Key == "Tgt.H") H = parameter.Value;
            }
        }
    }

    class ProjectileSimulator
    {
        public float L;
        public float Vm;
        public float By;
        private float Bx;
        public float Alpha;
        public float Gamma;
        public float TimeDelta;
        public uint TimeTicks;

        private float CosDx;
        private float CosDy;
        private float CosDz;

        private float Lx;
        private float Ly;
        private float Lz;

        private float Vx;
        private float Vy;
        private float Vz;

        private float Rh;
        private float Rw;
        private float Rl;


        public Target Tgt;
        public Terraine Trn;
        public Terraine Cnv;
        public Projectile Pjt;

        public ProjectileSimulator()
        {
            Tgt = new Target();
            Trn = new Terraine();
            Cnv = new Terraine();
            Pjt = new Projectile();
        }

        public static float Grads2Rads(float grads)
        {
            return grads * (float)Math.PI / 180.0f;
        }

        public const float Gravity = 9.82f;
        public const float Pi = (float)(Math.PI);
        public const float PiDiv2 = (float)(Math.PI * 0.5);
        public const float PiTimes2 = (float)(Math.PI * 2.0);

        public static float Cos(float rads) { return (float)Math.Cos((float)rads); }
        public static float Sin(float rads) { return (float)Math.Sin((float)rads); }

        public void CalculateProjectile(float time)
        {
            Pjt.Vx = Vx;
            Pjt.Vy = Vy;
            Pjt.Vz = Vz;

            Pjt.Sx = Lx + Pjt.Vx * time;
            Pjt.Sy = Ly + Pjt.Vy * time - 0.5f * Gravity * time * time;
            Pjt.Sz = Lz + Pjt.Vz * time;

            Pjt.Vy -= Gravity * time;

            //System.Diagnostics.Debug.WriteLine(
            //    "------------------------------------------\n" + 
            //    "Projectile => Time=" + time + "\n" + 
            //    "\tSx=" + Pjt.Sx + "\n" +
            //    "\tSy=" + Pjt.Sy + "\n" +
            //    "\tSz=" + Pjt.Sz + "\n" +
            //    "\tVx=" + Pjt.Vx + "\n" +
            //    "\tVy=" + Pjt.Vy + "\n" +
            //    "\tVz=" + Pjt.Vz
            //    );
        }

        public void GenerateTrajectory(out Polyline topView, out Polyline sideView)
        {
            sideView = new Polyline();
            topView = new Polyline();

            var timeMax = TimeDelta * TimeTicks;
            for (var time = 0.0f; time < timeMax; time += TimeDelta)
            {
                CalculateProjectile(time);

                if (Pjt.Sy < 0.0f)
                {
                    //System.Windows.MessageBox.Show("Out of range");
                    return;
                }

                System.Windows.Point sidePoint = new System.Windows.Point();
                System.Windows.Point topPoint = new System.Windows.Point();
                sidePoint.X = Pjt.Sx;
                sidePoint.Y = Cnv.H - Pjt.Sy;
                topPoint.X = Pjt.Sx;
                topPoint.Y = Pjt.Sz;

                sideView.Points.Add(sidePoint);
                topView.Points.Add(topPoint);
            }
        }

        public void OnWindowResized()
        {
            Rh = Cnv.H / Trn.H;
            Rw = Cnv.W / Trn.W;
            Rl = Cnv.L / Trn.L;
        }

        public void ReadParameters<T>(T parameters) where T : IEnumerable<Physics.ProjectileSimulatorConfigItem>
        {
            foreach(var parameter in parameters)
            {
                if (parameter.Key == "Sim.L") L = parameter.Value; else
                if (parameter.Key == "Sim.Vm") Vm = parameter.Value; else
                if (parameter.Key == "Sim.Yb") By = parameter.Value; else
                if (parameter.Key == "Sim.Alpha") Alpha = parameter.Value; else
                if (parameter.Key == "Sim.Gamma") Gamma = parameter.Value; else
                if (parameter.Key == "Sim.TimeDelta") TimeDelta = parameter.Value; else
                if (parameter.Key == "Sim.TimeTicks") TimeTicks = (uint)parameter.Value;
            }

            Tgt.ReadParameters(parameters);
            Trn.ReadParameters(parameters);
            OnWindowResized();

            Alpha = Grads2Rads(Alpha);
            Gamma = Grads2Rads(Gamma);

            Bx = L * Cos(PiDiv2 - Alpha);
            Lx = Bx * Cos(Gamma);
            Ly = L * Cos(Alpha);
            Lz = Bx * Sin(Gamma);

            CosDx = Lx / L;
            CosDy = Ly / L;
            CosDz = Lz / L;

            Vx = Vm * CosDx;
            Vy = Vm * CosDy;
            Vz = Vm * CosDz;
        }

    }
}
