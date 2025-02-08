class Image
{
public:
//TriangleInterpolated per a z buffer
  void DrawTriangleInterpolated(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Color& c0, const Color& c1, const Color& c2, FloatImage* zBuffer, const float depthValues[3]);
}
