float ax, ay, bx, by, cx, cy, dx, dy;
int g_scx, g_scy;

void initializeTriangle() { 
  ax = ay = 0.0f;
  bx = 0.0f;
  by = 100.0f;
  cx = 100.0f;
  cy = 0.0f;  
  dx = 50.0f;
  dy = 50.0f;
}

float crossProduct(float x1, float x2, float x3, float y1, float y2, float y3) { 
  return (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
}

boolean isInside()
{
  
  float dadb = crossProduct(ax, bx, dx, ay, by, dy); // DA-DB
  float dbdc = crossProduct(bx, cx, dx, by, cy, dy); // DB-DC
  float dcda = crossProduct(cx, ax, dx, cy, ay, dy); // DC-DA  
  return ((dadb * dbdc) < 0.0f) ||
    ((dbdc * dadb) < 0.0f) || 
    ((dcda * dadb) < 0.0f);  
}

void setup() { 
  initializeTriangle();  
  g_scx = 300;
  g_scy = 300;
  
  size(g_scx, g_scy);
  background(255);
}

void draw() { 
  background(255);
  translate(g_scx / 2, g_scy / 2);
  scale(1.0f, -1.0f);
  
  stroke(100, 100, 100);
  fill(0, 0, 100);
  line(-g_scx, 0.0f, g_scx, 0.0f);
  line(0.0f, -g_scy, 0.0f, g_scy);
  
  stroke(100, 100, 100);
  fill(0, 0, 100);
  line(ax, ay, bx, by);
  line(bx, by, cx, cy);
  line(ax, ay, cx, cy); 
  
  stroke(100, 100, 100);
  fill(0, 0, 100);
  ellipse(ax, ay, 10.0f, 10.0f);  
  ellipse(bx, by, 10.0f, 10.0f);  
  ellipse(cx, cy, 10.0f, 10.0f); 
  
  if (isInside()) {  
    fill(50, 0, 0);
    stroke(100, 0, 0);
    ellipse(dx, dy, 10.0f, 10.0f);    
  }
  else {
    fill(0, 50, 0);
    stroke(0, 100, 0);
    ellipse(dx, dy, 10.0f, 10.0f);   
  }
}

void mousePressed()
{
  dx = mouseX - g_scx / 2;
  dy = g_scy / 2 - mouseY;
}

