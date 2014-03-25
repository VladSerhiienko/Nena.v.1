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
