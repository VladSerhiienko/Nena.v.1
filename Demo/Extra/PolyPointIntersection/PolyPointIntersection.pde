
boolean g_testPointInside = false;
Point g_testPoint = null;
Polygon g_poly = null;
int g_numPoints = 12;
int g_scx = 300;
int g_scy = 300;

void setup() { 
  g_poly = new Polygon();
  g_testPoint = new Point();
  g_poly.points = new Point[g_numPoints];

  for (int ip = 0; ip < g_numPoints; ip++) { 
    g_poly.points[ip] = new Point();
    g_poly.points[ip].x = random(-100, 100);
    g_poly.points[ip].y = random(-100, 100);
    g_poly.points[ip].r = random(5, 15);
  }

  g_testPoint.r = 10;
  size(g_scx, g_scy);
}

boolean g_shuffle = false;
float g_tx = g_scx/2;
float g_ty = g_scy/2;
float g_sx = +1;
float g_sy = -1;
float g_mx = 0;
float g_my = 0;

void draw() { 
  background(0);  
  translate(g_tx, g_ty); 
  scale(g_sx, g_sy);
  stroke(255); 
  fill(0);

  if (g_shuffle) for (int i = 0; i < g_poly.PointsCount(); i++) { 
    g_poly.points[i].x += sin(random(-100, 100)); // * 0.5;
    g_poly.points[i].y += cos(random(-100, 100)); // * 0.5;
  }

  g_poly.Draw();  

  g_testPointInside = g_poly.IsInside(g_testPoint, 1000);
  if (g_testPointInside) fill(250, 0, 0);
  else fill(0, 250, 0); 
  
  g_testPoint.Draw();
}

void keyPressed() { 
  if (key == 'r' || key == 'R' ) { 
    if (g_shuffle) g_shuffle = false;
    else g_shuffle = true;
  }
}

void mouseDragged() {   
  g_mx = (mouseX - g_tx) * g_sx;
  g_my = (mouseY - g_ty) * g_sy;   
  if (mouseButton == LEFT) { 
    g_poly.OnMouseDragged(g_mx, g_my);
  } 
  else { 
    g_testPoint.x = g_mx;
    g_testPoint.y = g_my;
  }
}

