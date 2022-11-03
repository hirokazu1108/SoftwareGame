#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


#define WINDOW_PosX 0
#define WINDOW_PosY 0
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000


class Vector3{
    public:
        float x;
        float y;
        float z;
    Vector3(float a=0, float b=0, float c=0){x =a; y=b; z=c; }
    friend Vector3 operator+(Vector3 a, Vector3 b);
    friend Vector3 operator-(Vector3 a, Vector3 b);
    friend Vector3 operator*(Vector3 a, float k);
    friend Vector3 operator/(Vector3 a, float k);
    float Len(){return sqrt(x*x + y*y + z*z);}
};

class OBB
{
protected:
   Vector3 m_Pos;              // 位置
   Vector3 m_NormaDirect[3];   // 方向ベクトル
   float m_fLength[3];             // 各軸方向の長さ

public:
   void SetDirect(Vector3 v[3]){for(int i=0; i<3;i++){m_NormaDirect[i] = v[i];}};  // 指定軸番号の方向ベクトルを取得  
   void SetLen_W( float f[3]){for(int i=0; i<3;i++){m_fLength[i] = f[i];}};      // 指定軸方向の長さを取得
   void SetPos_W(Vector3 *a){m_Pos = *a;}             // 位置

   Vector3 GetDirect( int elem ){return m_NormaDirect[elem];}   // 指定軸番号の方向ベクトルを取得
   float GetLen_W( int elem ){return m_fLength[elem];}      // 指定軸方向の長さを取得
   Vector3 GetPos_W(){return m_Pos;}             // 位置を取得
};




extern float Dot(Vector3 *a, Vector3 *b);
extern float LenSegOnSeparateAxis( Vector3 *Sep, Vector3 *e1, Vector3 *e2, Vector3 *e3 = NULL);
extern Vector3 Cross(Vector3 *a, Vector3 *b);
extern bool ColOBBs( OBB &obb1, OBB &obb2 );

