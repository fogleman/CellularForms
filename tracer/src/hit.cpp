#include "hit.h"

void HittableList::Add(const P_Hittable &item) {
    m_Items.push_back(item);
}

void HittableList::AddLight(const P_Hittable &item) {
    m_Lights.push_back(item);
}

bool HittableList::Hit(const Ray &ray, const real tmin, const real tmax, HitInfo &hit) const {
    bool result = false;
    real closest = tmax;
    for (const auto &item : m_Items) {
        HitInfo temp;
        if (item->Hit(ray, tmin, closest, temp)) {
            result = true;
            closest = temp.T;
            hit = temp;
        }
    }
    return result;
}
