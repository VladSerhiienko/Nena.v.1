
Point p1 = new Point(); 
Point p2 = new Point();  
Point p3 = new Point(); 
Point p4 = new Point();   
LineSegment ls1 = new LineSegment();
LineSegment ls2 = new LineSegment();

int g_scx = 300, g_scy = 300; // screen dims

void setup() { 
  p1.name = "P1";
  p1.x = 0.0f;
  p1.y = 0.0f;
  p1.r = 10;
  
  p2.name = "P2";
  p2.x = 100.0f;
  p2.y = 100.0f;
  
  p3.name = "P3";
  p3.x = 0.0f;
  p3.y = 100.0f;
  p3.r = 10;
   
  p4.name = "P4";
  p4.x = 100.0f; //10.0f
  p4.y = 0.0f; //0.0f
  
  ls1.FromPoints(p1, p2);
  ls2.FromPoints(p3, p4);
  ls1.l.name = "Seg1Line";
  ls2.l.name = "Seg2Line";
  ls1.name = "Seg1";
  ls1.name = "Seg2"; 
  
  size(g_scx, g_scy);
  
}

Point g_cross = null;
float g_tx = g_scx/2;
float g_ty = g_scy/2;
float g_sx = +1;
float g_sy = -1;
void draw() { 
  background(0); translate(g_tx, g_ty); scale(g_sx, g_sy);
  stroke(255); fill(0);
  
  ls1.Draw();
  ls2.Draw();
  
  if (g_cross != null) { 
    fill(0, 250, 0);
    g_cross.r = 5.0f;
    g_cross.Draw();
  }  
}

float g_mx, g_my;
void mouseDragged() {   
  g_mx = (mouseX - g_tx) * g_sx;
  g_my = (mouseY - g_ty) * g_sy; 
  
  if (ls1.a.IsMouseOver(g_mx, g_my)) { ls1.a.x = g_mx; ls1.a.y = g_my; ls1.UpdateLine(); } else 
  if (ls1.b.IsMouseOver(g_mx, g_my)) { ls1.b.x = g_mx; ls1.b.y = g_my; ls1.UpdateLine(); } else
  if (ls2.a.IsMouseOver(g_mx, g_my)) { ls2.a.x = g_mx; ls2.a.y = g_my; ls2.UpdateLine(); } else
  if (ls2.b.IsMouseOver(g_mx, g_my)) { ls2.b.x = g_mx; ls2.b.y = g_my; ls2.UpdateLine(); }
  
  g_cross = ls1.CrossPointOf(ls2);
  //if (g_cross != null) print("cross" + g_cross.x + " " + g_cross.y + "\n");
}
