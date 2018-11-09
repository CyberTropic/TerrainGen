#ifndef CAMERA_H
#define CAMERA_H

#include "vectorOps.h"

class Camera
{
public:
  Camera();
  void orbitView(void);
  void setTargetObject(vec3* targetPos);
  void update();

  vec3 *camPos;
  vec3 *camTarget;

  float camDist;
  float camTwist;
  float camElev;
  float camAzimuth;
};

#endif