#ifndef GEOLOCATION_TOOLS_H_
#define GEOLOCATION_TOOLS_H_

#include <seekiosCore/seekios.h>
#include <peripheralManager/GPS_manager.h>
#include <statusManager/status_manager.h>
#include <stdbool.h>

void geolocationTools_constructVectors(Vector *vectors,int nbVectors, Coordinate *coordinates);
bool geolocationTools_isPositionInZone(Coordinate *seekiosLocationPtr,int nbCoordinates, Coordinate *zoneCoordinates);
double geolocationTools_getShortestDistanceFromEdge(double xA, double yA, double xB, double yB, Coordinate* point);
double geolocationTools_geoCoordinateToMeterDistance(double xA, double yA, double xB, double yB);
double geolocationTools_getShortestDistanceToZone(Coordinate* zoneCoordinates, uint8_t nbCoordinates, Coordinate* pointPtr);


#endif /* GEOLOCATION_TOOLS_H_ */