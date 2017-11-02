#include "Object3D.h"

#include <iostream>

using namespace std;

bool Sphere::intersect(const Ray &r, float tmin, Hit &h) const
{
    // Locate intersection point ( 2 pts )
    const Vector3f &rayOrigin = r.getOrigin(); //Ray origin in the world coordinate
    const Vector3f &dir = r.getDirection();

    Vector3f origin = rayOrigin - _center;      //Ray origin in the sphere coordinate

    float a = dir.absSquared();
    float b = 2 * Vector3f::dot(dir, origin);
    float c = origin.absSquared() - _radius * _radius;

    // no intersection
    if (b * b - 4 * a * c < 0) {
        return false;
    }

    float d = sqrt(b * b - 4 * a * c);

    float tplus = (-b + d) / (2.0f*a);
    float tminus = (-b - d) / (2.0f*a);

    // the two intersections are at the camera back
    if ((tplus < tmin) && (tminus < tmin)) {
        return false;
    }

    float t = 10000;
    // the two intersections are at the camera front
    if (tminus > tmin) {
        t = tminus;
    }

    // one intersection at the front. one at the back 
    if ((tplus > tmin) && (tminus < tmin)) {
        t = tplus;
    }

    if (t < h.getT()) {
        Vector3f normal = r.pointAtParameter(t) - _center;
        normal = normal.normalized();
        h.set(t, this->material, normal);
        return true;
    }
    return false;
}

// Add object to group
void Group::addObject(Object3D *obj) {
    m_members.push_back(obj);
}

// Return number of objects in group
int Group::getGroupSize() const {
    return (int)m_members.size();
}

bool Group::intersect(const Ray &r, float tmin, Hit &h) const
{
    bool hit = false;
    for (Object3D* o : m_members) {
        if (o->intersect(r, tmin, h)) {
            hit = true;
        }
    }
    return hit;
}


Plane::Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
    // TODO implement Plane constructor
    _normal = normal;
    _offset = d;
    _point = Vector3f( (normal[0] == 0 ? 0 : -d / _normal[0]), (normal[1] == 0 ? 0 : -d / _normal[1]), (normal[2] == 0 ? 0 : -d / _normal[2]));
    cout << "Constructing Plane... \nd:\t" << d << "\nNormal:\t";
    _normal.print();
    cout << "Point:\t";
    _point.print();
    
}
bool Plane::intersect(const Ray &r, float tmin, Hit &h) const
{
//    cout << "intersect()...\n";
//    r.getOrigin().print();
//    r.getDirection().print();
    float inter1 = (-1.f) * (_offset + Vector3f::dot(_normal, r.getOrigin()) );
//    cout << "inter1 : " << inter1 << "\n";
    float inter2 = Vector3f::dot(_normal, r.getDirection());
//    cout << "inter2 : " << inter2 << "\n";
    
    if(inter2 == 0) {
//        cout << "Returning " << (inter1 == 0 ? true : false) << "\n";
        return (inter1 == 0 ? true : false);
    }
    float t = inter1 / inter2;
    
//    cout << "computed t: \t " << t << "tmin: " << tmin << "\n";
    
    if( t > tmin ) {
        if (t < h.getT()) {
            Vector3f normal = r.pointAtParameter(t);
            normal = normal.normalized();
            h.set(t, this->material, normal);
//            cout << "returning true \n";
            return true;
        }
//        cout << "returning true \n";
//        return true;
    }
//    cout << "returning false \n";
    
    return false;
}

bool Triangle::intersect(const Ray &r, float tmin, Hit &h) const 
{
//    Plane p = Plane(_normals[0], 0, m);

    Matrix3f A = Matrix3f( ( _v[0].x() - _v[1].x() ), ( _v[0].x() - _v[2].x() ), ( r.getDirection().x() ),
                           ( _v[0].y() - _v[1].y() ), ( _v[0].y() - _v[2].y() ), ( r.getDirection().y() ),
                           ( _v[0].z() - _v[1].z() ), ( _v[0].z() - _v[2].z() ), (r.getDirection().z()) );
    float detA = A.determinant();
    float beta = Matrix3f(
                          (_v[0].x() - r.getOrigin().x()), (_v[0].x() - _v[2].x()), (r.getDirection().x()),
                          (_v[0].y() - r.getOrigin().y()), (_v[0].y() - _v[2].y()), (r.getDirection().y()),
                          (_v[0].z() - r.getOrigin().z()), (_v[0].z() - _v[2].z()), (r.getOrigin().z()) ).determinant() / detA;
//    cout << "Beta:\t" << beta << "\n";
    
    float gamma = Matrix3f(
                           (_v[0].x() - _v[1].x()), (_v[0].x() - r.getOrigin().x()), (r.getDirection().x()),
                           (_v[0].y() - _v[1].y()), (_v[0].y() - r.getOrigin().y()), (r.getDirection().y()),
                           (_v[0].z() - _v[1].z()), (_v[0].z() - r.getOrigin().z()), (r.getDirection().z()) ).determinant() / detA;
    
    
    float t = Matrix3f( ( _v[0].x() - _v[1].x() ), ( _v[0].x() - _v[2].x() ), ( _v[0].x() - r.getOrigin().x()),
                       ( _v[0].y() - _v[1].y() ), ( _v[0].y() - _v[2].y() ), ( _v[0].y() - r.getOrigin().y()),
                       ( _v[0].z() - _v[1].z() ), ( _v[0].z() - _v[2].z() ), ( _v[0].z() - r.getOrigin().z() ) ).determinant() / detA;
    
//    cout << "Gamma:\t" << gamma << "\n";
    
    float alpha = 1.0f - beta - gamma;
//    cout << "Alpha:\t" << alpha << "\n";
//    cout << "t:\t" << t << "\n";
    
    
    
    /*float area = areaFromPoints(_v[0], _v[1], _v[2]);
    cout << "Computed area = " << area << "\n";
    
    float alpha = areaFromPoints(r.getOrigin(), _v[1], _v[2]) / area;
    float beta = areaFromPoints(r.getOrigin(), _v[0], _v[2]) / area;
    float gamma = areaFromPoints(r.getOrigin(), _v[0], _v[1]) / area;
    
    cout << "\tArea:\t" << area << "\tAlpha:\t" << alpha << "\n\tBeta:\t" << beta << "\n\tGamma:\t" << gamma <<"\n";
    */
    
    bool inTriangle = (alpha >= 0 && beta >= 0 && gamma >= 0);
    if (t > tmin) {
        if (t < h.getT()) {
            Vector3f normal = r.pointAtParameter(t);
            normal = normal.normalized();
            h.set(t, this->material, normal);
            return true;
        }
//        return true;
    }
    return false;
}


Transform::Transform(const Matrix4f &m,
    Object3D *obj) : _object(obj) {
    // TODO implement Transform constructor
}
bool Transform::intersect(const Ray &r, float tmin, Hit &h) const
{
    // TODO implement
    return false;
}
