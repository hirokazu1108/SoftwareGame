#include"header.h"

Vector3 operator+(Vector3 a, Vector3 b){
    Vector3 c;
    c.x = a.x + b.x+2;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    return c;
}

Vector3 operator-(Vector3 a, Vector3 b){
    Vector3 c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    return c;
}

Vector3 operator*(Vector3 a, float k){
    Vector3 c;
    c.x = k*a.x;
    c.y = k*a.y;
    c.z = k*a.z;

    return c;
}

Vector3 operator/(Vector3 a, float k){
    Vector3 c;
    if(k == 0)
    {
        return a;
    }
    c.x = a.x/k;
    c.y = a.y/k;
    c.z = a.z/k;

    return c;
}

float Dot(Vector3 *a, Vector3 *b)
{
    return (a->x*b->x + a->y*b->y +a->z*b->z);
}

Vector3 Cross(Vector3 *a, Vector3 *b){
    Vector3 c;
    c.x = a->y*b->z - a->z*b->y;
    c.y = a->z*b->x - a->x*b->z;
    c.z = a->x*b->y - a->y*b->x;
    return c;
}

// ʬΥ������Ƥ��줿����ʬ���������ʬĹ�򻻽�
float LenSegOnSeparateAxis( Vector3 *Sep, Vector3 *e1, Vector3 *e2, Vector3 *e3 )
{
   // 3�Ĥ����Ѥ������ͤ��¤������ʬĹ��׻�
   // ʬΥ��Sep��ɸ�ಽ����Ƥ��뤳��
   float r1 = fabs(Dot( Sep, e1 ));
   float r2 = fabs(Dot( Sep, e2 ));
   float r3 = (e3 != NULL) ? (fabs(Dot( Sep, e3 ))) : 0;
   return r1 + r2 + r3;
}


bool ColOBBs( OBB &obb1, OBB &obb2 )
{
   // �������٥��ȥ�γ���
   // ��N***:ɸ�ಽ�����٥��ȥ��
   Vector3 NAe1 = obb1.GetDirect(0), Ae1 = NAe1 * obb1.GetLen_W(0);
   Vector3 NAe2 = obb1.GetDirect(1), Ae2 = NAe2 * obb1.GetLen_W(1);
   Vector3 NAe3 = obb1.GetDirect(2), Ae3 = NAe3 * obb1.GetLen_W(2);
   Vector3 NBe1 = obb2.GetDirect(0), Be1 = NBe1 * obb2.GetLen_W(0);
   Vector3 NBe2 = obb2.GetDirect(1), Be2 = NBe2 * obb2.GetLen_W(1);
   Vector3 NBe3 = obb2.GetDirect(2), Be3 = NBe3 * obb2.GetLen_W(2);
   Vector3 Interval = obb1.GetPos_W() - obb2.GetPos_W();

   // ʬΥ�� : Ae1
   float rA = Ae1.Len();
   float rB = LenSegOnSeparateAxis( &NAe1, &Be1, &Be2, &Be3 );
   float L = fabs(Dot( &Interval, &NAe1 ));
   if( L > rA + rB )
      return false; // ���ͤ��Ƥ��ʤ�

   // ʬΥ�� : Ae2
   rA = Ae2.Len();
   rB = LenSegOnSeparateAxis( &NAe2, &Be1, &Be2, &Be3 );
   L = fabs(Dot( &Interval, &NAe2 ));
   if( L > rA + rB )
   return false;

   // ʬΥ�� : Ae3
   rA = Ae3.Len();
   rB = LenSegOnSeparateAxis( &NAe3, &Be1, &Be2, &Be3 );
   L = fabs(Dot( &Interval, &NAe3 ));
   if( L > rA + rB )
      return false;

   // ʬΥ�� : Be1
   rA = LenSegOnSeparateAxis( &NBe1, &Ae1, &Ae2, &Ae3 );
   rB = Be1.Len();
   L = fabs(Dot( &Interval, &NBe1 ));
   if( L > rA + rB )
      return false;

   // ʬΥ�� : Be2
   rA = LenSegOnSeparateAxis( &NBe2, &Ae1, &Ae2, &Ae3 );
   rB = Be2.Len();
   L = fabs(Dot( &Interval, &NBe2 ));
   if( L > rA + rB )
      return false;

   // ʬΥ�� : Be3
   rA = LenSegOnSeparateAxis( &NBe3, &Ae1, &Ae2, &Ae3 );
   rB = Be3.Len();
   L = fabs(Dot( &Interval, &NBe3 ));
   if( L > rA + rB )
      return false;

   // ʬΥ�� : C11
   Vector3 cross;
   cross = Cross(&NAe1, &NBe1 );
   rA = LenSegOnSeparateAxis( &cross, &Ae2, &Ae3 );
   rB = LenSegOnSeparateAxis( &cross, &Be2, &Be3 );
   L = fabs(Dot( &Interval, &cross ));
   if( L > rA + rB )
      return false;

   // ʬΥ�� : C12
   cross = Cross( &NAe1, &NBe2 );
   rA = LenSegOnSeparateAxis( &cross, &Ae2, &Ae3 );
   rB = LenSegOnSeparateAxis( &cross, &Be1, &Be3 );
   L = fabs(Dot( &Interval, &cross ));
      if( L > rA + rB )
   return false;

   // ʬΥ�� : C13
   cross = Cross( &NAe1, &NBe3 );
   rA = LenSegOnSeparateAxis( &cross, &Ae2, &Ae3 );
   rB = LenSegOnSeparateAxis( &cross, &Be1, &Be2 );
   L = fabs(Dot( &Interval, &cross ));
   if( L > rA + rB )
      return false;

   // ʬΥ�� : C21
   cross = Cross( &NAe2, &NBe1 );
   rA = LenSegOnSeparateAxis( &cross, &Ae1, &Ae3 );
   rB = LenSegOnSeparateAxis( &cross, &Be2, &Be3 );
   L = fabs(Dot( &Interval, &cross ));
   if( L > rA + rB )
      return false;

   // ʬΥ�� : C22
   cross = Cross( &NAe2, &NBe2 );
   rA = LenSegOnSeparateAxis( &cross, &Ae1, &Ae3 );
   rB = LenSegOnSeparateAxis( &cross, &Be1, &Be3 );
   L = fabs(Dot( &Interval, &cross ));
   if( L > rA + rB )
      return false;

   // ʬΥ�� : C23
   cross = Cross( &NAe2, &NBe3 );
   rA = LenSegOnSeparateAxis( &cross, &Ae1, &Ae3 );
   rB = LenSegOnSeparateAxis( &cross, &Be1, &Be2 );
   L = fabs(Dot( &Interval, &cross ));
   if( L > rA + rB )
      return false;

   // ʬΥ�� : C31
   cross = Cross( &NAe3, &NBe1 );
   rA = LenSegOnSeparateAxis( &cross, &Ae1, &Ae2 );
   rB = LenSegOnSeparateAxis( &cross, &Be2, &Be3 );
   L = fabs(Dot( &Interval, &cross ));
   if( L > rA + rB )
      return false;

   // ʬΥ�� : C32
   cross = Cross( &NAe3, &NBe2 );
   rA = LenSegOnSeparateAxis( &cross, &Ae1, &Ae2 );
   rB = LenSegOnSeparateAxis( &cross, &Be1, &Be3 );
   L = fabs(Dot( &Interval, &cross ));
   if( L > rA + rB )
      return false;

   // ʬΥ�� : C33
   cross = Cross( &NAe3, &NBe3 );
   rA = LenSegOnSeparateAxis( &cross, &Ae1, &Ae2 );
   rB = LenSegOnSeparateAxis( &cross, &Be1, &Be2 );
   L = fabs(Dot( &Interval, &cross ));
   if( L > rA + rB )
      return false;

   // ʬΥʿ�̤�¸�ߤ��ʤ��Τǡ־��ͤ��Ƥ����
   return true;
}

