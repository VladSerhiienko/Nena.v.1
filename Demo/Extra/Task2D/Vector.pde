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
  public void Subtract( Vector other ) { 
    this.x -= other.x; 
    this.y -= other.y;
  }
  public void Add( Vector other ) { 
    this.x += other.x; 
    this.y += other.y;
  }
  public String toString() { 
    return "Vector: " + name + "(" + this.x + ", " + this.y + ")";
  }
}
