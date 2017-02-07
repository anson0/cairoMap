#include <stdint.h>
#include <stdlib.h>
//#include <math>
struct node
{
    uint64_t id;
    double lat;
    double lon;
    bool bHasWay;
    double distance;
    uint64_t idPrev;
    node()
    {
        bHasWay=false;
        idPrev=0;
        distance=0;
        id=0;

    }
    bool operator==(node b){return (b.id==this->id&&abs(b.lat-this->lat)<1e-10&&abs(b.lon-this->lon)<1e-10);}
    void operator=(node b){this->id=b.id;this->lat=b.lat;this->lon=b.lon;this->distance=b.distance;this->idPrev=b.idPrev;}
};


