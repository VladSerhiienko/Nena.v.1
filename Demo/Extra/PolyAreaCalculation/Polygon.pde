class Polygon { 
  public Point[] points;
  public String name;

  public Polygon() { 
    points = null;
  }

  public float CalculateArea() {
    Point testPoint = new Point();
    testPoint.x = 0.0f;
    testPoint.x = 0.0f;
    return CalculateArea(testPoint);
  }

  public float CalculateArea(Point testPoint) {     
    float area = 0.0f;
    for (int i = 0; i < PointsCount(); i++) {
      Vector a = new Vector();
      Vector b = new Vector();
      a.x = this.points[i].x - testPoint.x;
      a.y = this.points[i].y - testPoint.y;
      b.x = this.points[(i + 1) % PointsCount()].x - testPoint.x;
      b.y = this.points[(i + 1) % PointsCount()].y - testPoint.y;      
      area += a.CrossProduct(b) * 0.5f;      
    }
    
    return abs(area);
  }

  public boolean IsInside(final Polygon otherPoly, final float testDistance) {
    for (int i = 0; i < PointsCount(); i++) {
      final boolean isOutside = !IsInside(otherPoly.points[i], testDistance);
      if (isOutside) return false;
    } 
    return true;
  }

  // @arg startPoint - point to test
  // @arg testDistance - should be big enough for testing
  public boolean IsInside(final Point startPoint, final float testDistance) {
    Point endPoint = new Point();
    endPoint.x = startPoint.x + testDistance;
    endPoint.y = startPoint.y - testDistance;

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

  public void OnMouseDragged(final float mx, final float my) {
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
    this.DrawLines();
    this.DrawPoints();
  }  

  public String toString() {
    String result = new String("Polygon:");
    for (int i = 0; i < points.length; i++) 
      result += " " + points[i].toString();    
    return result;
  }
}

