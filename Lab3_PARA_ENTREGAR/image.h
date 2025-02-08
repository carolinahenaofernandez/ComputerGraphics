
class Image
{
   
public:
//Añade esto pookie
    void ScanLineDDA(int x0, int y0, int x1, int y1, std::vector<std::pair<int, int>>& table, int minY);
    void DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor);
}
