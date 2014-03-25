class Vector { 
  public float x, y;
  public String name;
  
  public Vector() { 
  }
  public Vector(final float x, final float y) {
   this.x = x; this.y = y; 
  }
  public Vector(final Vector other) {
   this.x = other.x; 
   this.y = other.y; 
  }
  public Vector(final Point p1, final Point p2) {
   FromPoints(p1, p2);
  }
  public void FromPoints(final Point p1, final Point p2) { 
    this.x = p2.x - p1.x;
    this.y = p2.y - p1.y;
  }
  public void FromLineSegment(final LineSegment ls) { 
    this.x = ls.b.x - ls.a.x;
    this.y = ls.b.y - ls.a.y;    
  }
  public float MagnitudeSqr() { 
    return this.x * this.x + this.y * this.y;
  }
  public float Magnitude() { 
    return sqrt(MagnitudeSqr());
  }
  public float DotProduct(final Vector other) { 
    return this.x * other.x + this.y * other.y;
  }
  public float CrossProduct(final Vector other) { 
    return this.x * other.y - other.x * this.y;
  }  
  public void NegateFrom(final Vector other) { 
    this.x = -other.x; 
    this.y = -other.y;
  } 
  public void Scale(final float factor) { 
    this.x *= factor; 
    this.y *= factor;
  }
  public void MultiplyOn(final Vector other) { 
    this.x *= other.x; 
    this.y *= other.y;
  }
  public void DivideOn(final Vector other) { 
    this.x /= other.x; 
    this.y /= other.y;
  }
  public void Add(final Vector other) { 
    this.x += other.x; 
    this.y += other.y;
  }
  public void Subtract(final Vector other) { 
    this.x -= other.x; 
    this.y -= other.y;
  }
  public String toString() { 
    return "Vector: " + name + "(" + 
      this.x + ", " + 
      this.y + ")";
  }
}
