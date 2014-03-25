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
