
/* --------------------------------------------------------------------
 * gls04.c
 * 2個の立方体を別々に動かす
 * -------------------------------------------------------------------- */
//test4
/* ヘッダファイル */
#include "header.h"

#define RAD (M_PI / 180.0)

typedef struct {
    float ShiftX;
    float ShiftY;
    float ShiftZ;
    float RotateX;
    float RotateY;
    float RotateZ;
} Geometry;
Geometry Cube;

/* グローバル変数 */
int xBegin            = 0;    /* マウスドラッグの始点X座標 */
int yBegin            = 0;    /* マウスドラッグの始点Y座標 */
int PressButton       = 0;    /* 現在押されているマウスボタン（1:左,2:中,3:右） */
float CameraAzimuth   = 90.0; /* カメラの位置（方位角） */
float CameraElevation = 0.0;  /* カメラの位置（仰角） */
float CameraDistance  = 5.0;  /* カメラの位置（原点からの距離） */
float CameraX         = 5.0;  /* カメラの位置（X座標） */
float CameraY         = 3.0;  /* カメラの位置（Y座標） */
float CameraZ         = 0.0;  /* カメラの位置（Z座標） */

char buf[256];

GLdouble vertex[][3] = {
  { 0.0, 0.0, 0.0 }, /* A */
  { 1.0, 0.0, 0.0 }, /* B */
  { 1.0, 1.0, 0.0 }, /* C */
  { 0.0, 1.0, 0.0 }, /* D */
  { 0.0, 0.0, 1.0 }, /* E */
  { 1.0, 0.0, 1.0 }, /* F */
  { 1.0, 1.0, 1.0 }, /* G */
  { 0.0, 1.0, 1.0 }  /* H */
};

GLdouble vertex2[][3] = {
  { 0.0, 0.0, 3.0 }, /* A */
  { 1.0, 0.0, 3.0 }, /* B */
  { 1.0, 1.0, 3.0 }, /* C */
  { 0.0, 1.0, 3.0 }, /* D */
  { 0.0, 0.0, 4.0 }, /* E */
  { 1.0, 0.0, 4.0 }, /* F */
  { 1.0, 1.0, 4.0 }, /* G */
  { 0.0, 1.0, 4.0 }  /* H */
};

OBB vertexToOBB(GLdouble vert[][3]){
    OBB obb;
    Vector3 m_pos;
    Vector3 d[3];
    float len[3];
    
    d[0].x = vert[1][0] - vert[0][0];
    d[0].y = vert[1][1] - vert[0][1];
    d[0].z = vert[1][2] - vert[0][2];
    d[1].x = vert[3][0] - vert[0][0];
    d[1].y = vert[3][1] - vert[0][1];
    d[1].z = vert[3][2] - vert[0][2];
    d[2].x = vert[4][0] - vert[0][0];
    d[2].y = vert[4][1] - vert[0][1];
    d[2].z = vert[4][2] - vert[0][2];
    m_pos.x = vert[0][0] + d[0].x/2;
    m_pos.y = vert[0][1] + d[0].y/2;
    m_pos.z = vert[0][2] + d[0].z/2;
    for(int i = 0; i < 3; i++)
    {
        len[i] = d[i].Len();
        d[i] = d[i] / len[i];
        len[i] /= 2;
    }
    
    obb.SetDirect(d);
    obb.SetLen_W(len);
    obb.SetPos_W(&m_pos);

    return obb;
}

int edge[][2] = {
  { 0, 1 }, /* ア (A-B) */
  { 1, 2 }, /* イ (B-C) */
  { 2, 3 }, /* ウ (C-D) */
  { 3, 0 }, /* エ (D-A) */
  { 4, 5 }, /* オ (E-F) */
  { 5, 6 }, /* カ (F-G) */
  { 6, 7 }, /* キ (G-H) */
  { 7, 4 }, /* ク (H-E) */
  { 0, 4 }, /* ケ (A-E) */
  { 1, 5 }, /* コ (B-F) */
  { 2, 6 }, /* サ (C-G) */
  { 3, 7 }  /* シ (D-H) */
};

int face[][4] = {
  { 0, 1, 2, 3 },
  { 1, 5, 6, 2 },
  { 5, 4, 7, 6 },
  { 4, 0, 3, 7 },
  { 4, 5, 1, 0 },
  { 3, 2, 6, 7 }
};

/* 関数のプロトタイプ宣言 */
void display(void);
void timer(int timerID);
void keyboard(unsigned char key, int x, int y);
void myInit(char *windowTitle);
void resize(int w, int h);

#define TEX_HEIGHT 32
#define TEX_WIDTH 32
static GLubyte image[TEX_HEIGHT][TEX_WIDTH][4];

/***********************************************************
|  関数：main()
|  説明：メイン関数
|  引数：int argc       実行時引数の数
|  引数：char** argv    実行時引数の内容（文字列配列）
|  戻値：int            0:正常終了
***********************************************************/
int main(int argc, char **argv)
{
    /* 初期化 */
    glutInit(&argc, argv); /* OpenGL の初期化 */
    myInit(argv[0]);       /* ウインドウ表示と描画設定の初期化 */
    
    Cube.ShiftX  = 0;
    Cube.ShiftY  = 0;
    Cube.ShiftZ  = 0;
    Cube.RotateX = 0;
    Cube.RotateY = 0;
    Cube.RotateZ = 0;

    glutMainLoop();

    /* プログラム終了 */
    return 0;
}

void resize(int w, int h)
{
    /* ウインドウの縦横の比を計算 */
    float aspect = (float)w/ (float)h;
    /* ウィンドウ全体をビューポートする */
    glViewport(0,0,w,h);

    /* CG描画設定 */
    glMatrixMode(GL_PROJECTION);             /* 透視投影(遠近投影法)設定モードに切り替え */
    glLoadIdentity();                        /* 透視投影行列を初期化 */
    gluPerspective(45.0, aspect, 1.0, 20.0); /* 透視投影行列の設定 */
                                             /* 視野角45度, 縦横比 aspect，描画前面までの奥行 1.0，描画背面までの奥行 20.0 */
    /* モデルビュー変換行列の設定 */
    glMatrixMode(GL_MODELVIEW);
}

/**********************************************************
|  関数：display()
|  説明：「１枚の」グラフィック描画イベント処理
|  引数：なし
|  戻値：なし
***********************************************************/
void display(void)
{
    int i;

    /* 初期化 */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* 画面を消去 */
    glLoadIdentity();                                   /* 幾何を初期化する */

    gluLookAt(CameraX , CameraY, CameraZ, /* カメラの位置 */
        -10, -1.0, 0.0,                         /* 注視点の位置 */
        -10.0, 1.0, 0.0);                              /* カメラ上方向のベクトル */


   

    //床の表示
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(0.0, -0.8, 0.0);
    glScalef(1000.0, 0.1, 1.1);
    glutSolidCube(1.0);
    glPopMatrix();

 /* 立方体の描画 */
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.5, 0.5);
    for (int j = 0; j < 6; ++j) {
        for (int i = 0; i < 4; ++i) {
            glVertex3dv(vertex[face[j][i]]);
        }
    }
    glEnd();

    glPopMatrix();
    glBegin(GL_QUADS);
    glColor3f(1, 0.5, 0.5);
    for (int j = 0; j < 6; ++j) {
        for (int i = 0; i < 4; ++i) {
            glVertex3dv(vertex2[face[j][i]]);
        }
    }
    glEnd();

    /* 上記で描画されたCGをモニターに出力 */
    glutSwapBuffers();
}


/***********************************************************
|  関数：timer(int timerID)
|  説明：タイマー（設定時間経過）イベント処理
|  引数：int timerID    イベントが発生したタイマーの識別ID
|  戻値：なし
***********************************************************/
void timer(int timerID)
{
    int i;

    /* 次のタイマーを15ミリ秒後に設定 */
    glutTimerFunc(15, timer, 0);

    /* 描画要求（直後に display() 関数が呼ばれる） */
    glutPostRedisplay();
    // SDL_Delay(10);
}

/***********************************************************
|  関数：keyboard()
|  説明：キーボードが押された時のイベント処理
|  引数：unsigned char key  押されたキーの文字コード
|  引数：int x              キーが押されたときのマウスポインタのX座標
|  引数：int y              キーが押されたときのマウスポインタのY座標
|  戻値：なし
***********************************************************/
void keyboard(unsigned char key, int x, int y)
{

    /* キーボード処理 */
    switch (key) {
    case 'a':
        for(int i = 0; i < 8; i++)
            vertex2[i][2] += 0.1;
        break;
    case 'd':
        for(int i = 0; i < 8; i++)
            vertex2[i][2] -= 0.1;
        break;
    case 'w':
        for(int i = 0; i < 8; i++)
            vertex2[i][0] -= 0.1;
        break;
    case 's':
        for(int i = 0; i < 8; i++)
            vertex2[i][0] += 0.1;
        break;
    case 'n':
        for(int i = 0; i < 8; i++)
            vertex2[i][1] += 0.1;
        break;
    case 'm':
        for(int i = 0; i < 8; i++)
            vertex2[i][1] -= 0.1;
        break;
    default:
        break;
    }
    OBB obb1,obb2;
    obb1 = vertexToOBB(vertex);
    obb2 = vertexToOBB(vertex2);

    if(ColOBBs(obb1,obb2)){
        printf("hit\n");
    }
    

    /* 描画要求（直後に display() 関数が呼ばれる） */
    glutPostRedisplay();
}


/***********************************************************
|  関数：myInit()
|  説明：ウインドウ表示と描画設定の初期化
|  引数：char *windowTitle      ウインドウのタイトルバーに表示する文字列
|  戻値：なし
***********************************************************/
void myInit(char *windowTitle)
{
    /* OpenGLウインドウ作成までの初期化 */
    glutInitWindowPosition(0, 0);            /* ウインドウ表示位置 */
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); /* ウインドウサイズ */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);   /* 描画モード  GLUT_DEPTHは処理が重くなりうるので必要なときのみ使う glEnable(GL_DEPTH_TEST)-- glDisable(GL_DEPTH_TEST)*/

    glutCreateWindow(windowTitle);                      /* ウインドウの表示 */
    glClearColor(0.0, 0.0, 0.0, 1.0);                   /* 画面消去色の設定 */

    /* イベント発生時に呼び出す関数の登録 */
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);  /* キーボードを押した時 */
    glutDisplayFunc(display);    /* 画面表示 */
    glutTimerFunc(15, timer, 0); /* タイマーを15ミリ秒後に設定 */

    
    /*
    glEnable(GL_DEPTH_TEST);                 // 隠面消去を有効にする
    glEnable(GL_CULL_FACE); //3カリング　閉じた立体の視点に対してウラ面をみえなくする1
    glCullFace(GL_BACK);//3カリング　閉じた立体の視点に対してウラ面をみえなくする2
    glDepthFunc(GL_LEQUAL);
    initTexture();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_WIDTH, TEX_HEIGHT,
        0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    */
}

