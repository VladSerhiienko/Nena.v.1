import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class PolyPointIntersection extends PApplet {



boolean g_testPointInside = false;
Point g_testPoint = null;
Polygon g_poly = null;
int g_numPoints = 6;
int g_scx = 300, g_scy = 300; // screen dims

public void setup() { 
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

public void draw() { 
  background(0);  
  translate(g_tx, g_ty); 
  scale(g_sx, g_sy);
  stroke(255); 
  fill(0);

  for (int i = 0; i < g_poly.PointsCount(); i++)  { 
    g_poly.points[i].x += sin(random(-100, 100)) * 0.5f;
    g_poly.points[i].y += cos(random(-100, 100)) * 0.5f;
  }
  
  g_poly.Draw();  

  if (g_testPointInside) fill(250, 0, 0);
  else fill(0, 250, 0);  
  g_testPoint.Draw();
}

public void keyPressed() { 
  //if (key == '
  
}

public void mouseDragged() {   
  g_mx = (mouseX - g_tx) * g_sx;
  g_my = (mouseY - g_ty) * g_sy;   
  if (mouseButton == LEFT) { 
    g_poly.OnMouseDragged(g_mx, g_my);
  } 
  else { 
    g_testPoint.x = g_mx;
    g_testPoint.y = g_my;     
    g_testPointInside = g_poly.IsInside(
    g_testPoint, 1000
      );
  }
}

class Line { 
  public float k, b; // kx + b = y  
  public String name;
  
  public void FromPoints( Point a, Point b ) { 
    float dx = (b.x - a.x);
    if (dx != 0) this.k = (b.y - a.y) / dx;
    else this.k = 0.0f;
    this.b = a.y - a.x * this.k;
  }   
  public void CopyFrom( Line other ) { 
    this.name = other.name;
    this.k = other.k;
    this.b = other.b;
  }
  public boolean IsSameAs( Line other ) { 
    return other.k == this.k && other.b == this.b;
  }
  public boolean IsParallelTo( Line other ) { 
    return other.k == this.k;
  }
  public boolean IsPerpendicularTo( Line other ) { 
    return -1 == other.k * this.k;    
  }
  public boolean Crosses( Line other ) { 
    return other.k != this.k;
  } 
  public float Substitute( Point p ) { 
    return this.k * p.x + this.b;
  }
  public Point CrossPointOf( Line line ) { 
    if (this.IsParallelTo(line)) return null; else { 
      Point crossPoint = new Point();
      crossPoint.x = (line.b - this.b) / (this.k - line.k);
      crossPoint.y = this.Substitute(crossPoint);
      return crossPoint;
    }
  } 
  public String toString() { 
    return "Line: " + name + "(" + this.k + ", " + this.b + ")";
  }
}
class LineSegment { 
  public Point a, b; // points
  public Line l; // line
  public String name;
  
  public LineSegment() { 
    this.a = new Point(); // alloc
    this.b = new Point(); // alloc
    this.l = new Line(); // alloc
  }  
  public LineSegment(Point a, Point b) { 
    this(); // alloc 2 points and line
    FromPoints(a, b); // initialize
  }
  public void FromPoints( Point a, Point b ) { 
    this.a.CopyFrom(a);
    this.b.CopyFrom(b);
    this.l.FromPoints(a, b);
  }  
  public void UpdateLine() { 
    this.l.FromPoints(a, b); 
  }  
  public boolean Crosses( LineSegment ls ) {    
    if (this.l.IsParallelTo(ls.l)) return false;
    
    Vector p1p2 = new Vector(); /*alloc*/ p1p2.FromLineSegment(this); /*init*/
    Vector p3p4 = new Vector(); /*alloc*/ p3p4.FromLineSegment(ls); /*init*/
    Vector p1p3 = new Vector(); /*alloc*/ p1p3.FromPoints(this.a, ls.a); /*init*/
    Vector p1p4 = new Vector(); /*alloc*/ p1p4.FromPoints(this.a, ls.b); /*init*/
    Vector p3p1 = new Vector(); /*alloc*/ p3p1.FromPoints(ls.a, this.a); /*init*/
    Vector p3p2 = new Vector(); /*alloc*/ p3p2.FromPoints(ls.a, this.b); /*init*/
    
    // 1st case - both (p3, p4) points should lay on the different sides of <p1p2>
    // this far their cross products product should be negative
    
    float cross_p1p2_p1p3 = p1p2.CrossProduct(p1p3);    
    float cross_p1p2_p1p4 = p1p2.CrossProduct(p1p4);
    boolean bp1p2 = (cross_p1p2_p1p3 * cross_p1p2_p1p4) < -0.0f;
    
    // 2nd case - both points (p1, p2) should lay on the different sides of <p3p4>
    // this far their cross products product should be negative
    
    float cross_p3p4_p3p1 = p3p4.CrossProduct(p3p1);    
    float cross_p3p4_p3p2 = p3p4.CrossProduct(p3p2);
    boolean bp3p4 = (cross_p3p4_p3p1 * cross_p3p4_p3p2) < -0.0f;
    
    return bp1p2 && bp3p4;
  }
  public Point CrossPointOf( LineSegment ls ) { 
    if (!this.Crosses(ls)) return null; else { 
      return this.l.CrossPointOf(ls.l);
    }
  } 
  public void Draw() {  
    line(this.a.x, this.a.y, this.b.x, this.b.y);
    this.a.Draw();   
    this.b.Draw();   
  }
  
  public String toString() { 
    return "LineSegment: [" + 
      a.toString() + " " + 
      b.toString() + " " + 
      l.toString() + "]";
  }
}
class Point { 
  public float r; // circle radius (rendering)
  public float x, y; // point coordinates 
  public String name; // point name
  
  public Point() { 
    this.r = 5;
  }
  
  public boolean BelongsTo( Line line ) { 
    return SubstituteIn(line) == this.y;
  }  
  public float SubstituteIn( Line line ) { 
    return this.x * line.k + line.b;
  }  
  public void CopyFrom( Point other ) { 
    this.name = other.name;
    this.r = other.r;
    this.x = other.x;
    this.y = other.y;
  }
  public float DistanceTo( Point other ) { 
    Vector direction = new Vector(this, other);
    return direction.Magnitude();
  }
  public float DistanceTo( float x, float y ) { 
    Vector direction = new Vector(x - this.x, y - this.y);
    return direction.Magnitude();
  }
  public boolean IsMouseOver(float mx, float my) { 
    Point mouse = new Point();
    mouse.x = mx; mouse.x = my;
    return this.DistanceTo(mx, my) <= r;    
  }
  public boolean IsMouseOver(Point mouse) { 
    return this.DistanceTo(mouse) <= r;
  }
  public String toString() { 
    return "Point: " + name + "(" + this.x + ", " + this.y + ")";
  }
  public void Draw() { 
    ellipse(this.x, this.y, this.r, this.r);    
  }
}
class Polygon { 
  public Point[] points;
  public String name;

  public Polygon() { 
    points = null;
  }

  // @arg startPoint - point to test
  // @arg testDistance - should be big enough for testing
  public boolean IsInside(Point startPoint, float testDistance) {  
    Point endPoint = new Point();
    endPoint.x = startPoint.x + testDistance;
    endPoint.y = startPoint.y;

    LineSegment chkLs = new LineSegment();
    LineSegment tmpLs = new LineSegment();

    chkLs.a = startPoint;
    chkLs.b = endPoint;
    chkLs.UpdateLine();

    int timesCrossOccured = 0;
    for (int i = 0; i < PointsCount(); i++) {       
      tmpLs.b = this.points[(i + 1) % PointsCount()];
      tmpLs.a = this.points[i];
      tmpLs.UpdateLine();

      if (chkLs.Crosses(tmpLs)) { 
        timesCrossOccured++;
      }
    }
    
    return timesCrossOccured % 2 == 1;
  }

  public int PointsCount() { 
    return this.points.length;
  }  
  public int LinesCount() { 
    return this.points.length - 1;
  } 
  
  public void OnMouseDragged(float mx, float my) { 
    for (int i = 0; i < PointsCount(); i++) {
      if (this.points[i].IsMouseOver(mx, my)) {
        this.points[i].x = mx;
        this.points[i].y = my;
        return;
      }
    }
  }

  public void DrawLines() {  
    for (int i = 0; i < PointsCount(); i++) 
      line(this.points[i].x, this.points[i].y, 
        this.points[(i + 1) % PointsCount()].x, 
        this.points[(i + 1) % PointsCount()].y
        );
  }  
  public void DrawPoints() {
    for (int i = 0; i < PointsCount(); i++) 
      points[i].Draw();
  }  
  public void Draw() { 
    DrawLines();
    DrawPoints();
  }  

  public String toString() {
    String result = new String("Polygon:");
    for (int i = 0; i < points.length; i++) 
      result += " " + points[i].toString();    
    return result;
  }
}

class Vector { 
  public float x, y;
  public String name;
  
  public Vector() { 
  }
  public Vector(float x, float y) {
   this.x = x; this.y = y; 
  }
  public Vector( Vector other ) {
   this.x = other.x; 
   this.y = other.y; 
  }
  public Vector(  Point p1, Point p2 ) {
   FromPoints(p1, p2);
  }
  public void FromPoints( Point p1, Point p2 ) { 
    this.x = p2.x - p1.x;
    this.y = p2.y - p1.y;
  }
  public void FromLineSegment( LineSegment ls ) { 
    this.x = ls.b.x - ls.a.x;
    this.y = ls.b.y - ls.a.y;    
  }
  public float MagnitudeSqr() { 
    return this.x * this.x + this.y * this.y;
  }
  public float Magnitude() { 
    return sqrt(MagnitudeSqr());
  }
  public float DotProduct( Vector other ) { 
    return this.x * other.x + this.y * other.y;
  }
  public float CrossProduct( Vector other ) { 
    return this.x * other.y - other.x * this.y;
  }  
  public void NegateFrom( Vector other ) { 
    this.x = -other.x; 
    this.y = -other.y;
  } 
  public void Scale( float factor ) { 
    this.x *= factor; 
    this.y *= factor;
  }
  public void MultiplyOn( Vector other ) { 
    this.x *= other.x; 
    this.y *= other.y;
  }
  public void DivideOn( Vector other ) { 
    this.x /= other.x; 
    this.y /= other.y;
  }
  public void Add( Vector other ) { 
    this.x += other.x; 
    this.y += other.y;
  }
  public void Subtract( Vector other ) { 
    this.x -= other.x; 
    this.y -= other.y;
  }
  public String toString() { 
    return "Vector: " + name + "(" + this.x + ", " + this.y + ")";
  }
}
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "PolyPointIntersection" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
