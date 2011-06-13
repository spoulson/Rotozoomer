#pragma once

// Floating point equivalent of POINT
class FPoint {
public:
   double x, y;

   FPoint(void) { }
   FPoint(double x, double y) : x(x), y(y) { }
   FPoint(const FPoint &source) : x(source.x), y(source.y) { }

   FPoint operator=(const FPoint &source) {
      x = source.x;
      y = source.y;
      return *this;
   }

   bool operator==(const FPoint &rparam) const {
      return x == rparam.x && y == rparam.y;
   }

   bool operator!=(const FPoint &rparam) const {
      return x != rparam.x || y != rparam.y;
   }
};
