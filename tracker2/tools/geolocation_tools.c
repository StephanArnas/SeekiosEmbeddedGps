#include <tools/geolocation_tools.h>
#define _USE_MATH_DEFINES //l'ordre doit etre conservé
#include <math.h>
static double DDtoRad(double coordinateToRAD);
static int crossProduct(Vector vector1,Vector vector2);

void geolocationTools_constructVectors(Vector *vectors,int nbVectors, Coordinate *coordinates)
{
	for (int i=0;i<nbVectors;i++)
	{
		if(i<(nbVectors-1))
		{
			vectors[i].x=(coordinates[i+1].lat-coordinates[i].lat);
			vectors[i].y=(coordinates[i+1].lon-coordinates[i].lon);
		}
		else
		{
			vectors[i].x=(coordinates[0].lat-coordinates[i].lat);
			vectors[i].y=(coordinates[0].lon-coordinates[i].lon);
		}
	}
}

//Returns true if the location is in the zone, false otherwise
bool geolocationTools_isPositionInZone(Coordinate *seekiosLocationPtr, int nbCoordinates, Coordinate *zoneCoordinates)
{
	Vector zoneVectors[NB_MAX_COORDINATES];
	geolocationTools_constructVectors(zoneVectors, nbCoordinates, zoneCoordinates);
	
	Vector condition2_1;
	Vector condition2_2;
	Vector condition3_1;
	Vector condition3_2;
	Vector seekios;
		
	int index=0;
	/* Le point pris arbitrairement en dehors de la zone est le point de lat=0 et long =0.
	Les composantes du vecteur entre le Seekios et ce point sont donc égales aux coord
	du Seekios lui même. Il en est de meme pour le vecteur "condition3_1"
	*/
	seekios.x=-(seekiosLocationPtr->lat);
	seekios.y=-(seekiosLocationPtr->lon);
	volatile uint32_t zoneWm = uxTaskGetStackHighWaterMark(NULL);
	UNUSED(zoneWm);
	
	for (int i=0;i<nbCoordinates;i++)
	{
		//Création de vecteurs temporaires dépendant de i
		condition2_2.x=zoneCoordinates[i].lat-seekiosLocationPtr->lat;
		condition2_2.y=zoneCoordinates[i].lon-seekiosLocationPtr->lon;
		condition3_1.x=-zoneCoordinates[i].lat;
		condition3_1.y=-zoneCoordinates[i].lon;
		condition3_2.x=seekiosLocationPtr->lat-zoneCoordinates[i].lat;
		condition3_2.y=seekiosLocationPtr->lon-zoneCoordinates[i].lon;
		condition2_1.x=zoneCoordinates[(i+1)%nbCoordinates].lat-seekiosLocationPtr->lat;
		condition2_1.y=zoneCoordinates[(i+1)%nbCoordinates].lon-seekiosLocationPtr->lon;
		
		zoneWm = uxTaskGetStackHighWaterMark(NULL);
		if(crossProduct(seekios,zoneVectors[i])!=0
		&& (crossProduct(seekios,condition2_1)*crossProduct(seekios,condition2_2))<=0
		&&(crossProduct(zoneVectors[i],condition3_1)*crossProduct(zoneVectors[i],condition3_2))<=0)
		{
			index++;
		}
	}
volatile uint32_t wm=uxTaskGetStackHighWaterMark(NULL);
	return !((index%2)==0); // nombre d'intersections pair : seekios OUT, impair : IN
}


/* Gets the shortest distance between a point and a zone */
double geolocationTools_getShortestDistanceToZone(Coordinate* zoneCoordinates, uint8_t nbCoordinates, Coordinate* pointPtr){
	double shortestDistance = geolocationTools_getShortestDistanceFromEdge(zoneCoordinates[0].lat, zoneCoordinates[0].lon, zoneCoordinates[1].lat, zoneCoordinates[1].lon, pointPtr);
	uint8_t nbEdges = nbCoordinates;
	for(int i=1; i<nbEdges; i++){
		volatile double distance = geolocationTools_getShortestDistanceFromEdge(
			zoneCoordinates[i].lat,
			zoneCoordinates[i].lon,
			zoneCoordinates[(i+1)%nbEdges].lat,
			zoneCoordinates[(i+1)%nbEdges].lon,
			pointPtr
		);

		if(distance < shortestDistance){
			shortestDistance = distance;
		}
	}
	return shortestDistance;
}

/* Gets the shortest distance from a point to an edge of a zone*/
double geolocationTools_getShortestDistanceFromEdge(double xA, double yA, double xB, double yB, Coordinate* point){
	double A = point->lat - xA;
	double B = point->lon - yA;
	double C = xB - xA;
	double D = yB - yA;

	double dot = A*C + B*D;
	double squareLength = C*C + D*D;
	double param = -1;
	if(squareLength != 0) // edge length = 0
	{
		param = dot/squareLength;
	}

	double xH, yH;
	if(param < 0){
		xH = xA;
		yH = yA;
		} else if(param > 1){
		xH = xB;
		yH = yB;
		} else {
		xH = xA + param * C;
		yH = yA + param * D;
	}

	double distanceFromEdge = geolocationTools_geoCoordinateToMeterDistance(point->lat, point->lon, xH, yH);
	return distanceFromEdge;
}

/* From the geographical coordinates (lat and lon), converts it to a metrical distance */
double geolocationTools_geoCoordinateToMeterDistance(double xA, double yA, double xB, double yB){

	double xA_rad = DDtoRad(xA);
	double xB_rad = DDtoRad(xB);
	double yA_rad = DDtoRad(yA);
	double yB_rad = DDtoRad(yB);

	double sinXA = sin(xA_rad);
	double sinXB = sin(xB_rad);
	double cosXA = cos(xA_rad);
	double cosXB = cos(xB_rad);
	double cosDiffYAYB = cos(yA_rad	- yB_rad);

	return	abs(acos(sinXA*sinXB+cosXA*cosXB*cosDiffYAYB)*6371*1000);
}

//On convertit une coordonnée en RADIAN
static double DDtoRad(double coordinateToRAD)
{
	volatile double res=0.0;
	res=M_PI*(coordinateToRAD)/180;
	
	return res;
}

/* Effectue un produit en croix. Retourne -1 si résultat < 0, 1 si résultat > 1 */
static int crossProduct(Vector vector1,Vector vector2)
{
	double result=vector1.x*vector2.y-vector1.y*vector2.x;
	if(result<0)
	{
		return -1;
	}
	else if (result>0)
	{
		return 1;
	}
	else return 0;
}