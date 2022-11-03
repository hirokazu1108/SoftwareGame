
/* --------------------------------------------------------------------
 * gls04.c
 * 2�Ĥ�Ω���Τ��̡���ư����
 * -------------------------------------------------------------------- */

/* �إå��ե����� */
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

/* �����Х��ѿ� */
int xBegin            = 0;    /* �ޥ����ɥ�å��λ���X��ɸ */
int yBegin            = 0;    /* �ޥ����ɥ�å��λ���Y��ɸ */
int PressButton       = 0;    /* ���߲�����Ƥ���ޥ����ܥ����1:��,2:��,3:���� */
float CameraAzimuth   = 90.0; /* �����ΰ��֡����̳ѡ� */
float CameraElevation = 0.0;  /* �����ΰ��֡ʶĳѡ� */
float CameraDistance  = 5.0;  /* �����ΰ��֡ʸ�������ε�Υ�� */
float CameraX         = 5.0;  /* �����ΰ��֡�X��ɸ�� */
float CameraY         = 3.0;  /* �����ΰ��֡�Y��ɸ�� */
float CameraZ         = 0.0;  /* �����ΰ��֡�Z��ɸ�� */

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
  { 0, 1 }, /* �� (A-B) */
  { 1, 2 }, /* �� (B-C) */
  { 2, 3 }, /* �� (C-D) */
  { 3, 0 }, /* �� (D-A) */
  { 4, 5 }, /* �� (E-F) */
  { 5, 6 }, /* �� (F-G) */
  { 6, 7 }, /* �� (G-H) */
  { 7, 4 }, /* �� (H-E) */
  { 0, 4 }, /* �� (A-E) */
  { 1, 5 }, /* �� (B-F) */
  { 2, 6 }, /* �� (C-G) */
  { 3, 7 }  /* �� (D-H) */
};

int face[][4] = {
  { 0, 1, 2, 3 },
  { 1, 5, 6, 2 },
  { 5, 4, 7, 6 },
  { 4, 0, 3, 7 },
  { 4, 5, 1, 0 },
  { 3, 2, 6, 7 }
};

/* �ؿ��Υץ�ȥ�������� */
void display(void);
void timer(int timerID);
void keyboard(unsigned char key, int x, int y);
void myInit(char *windowTitle);
void resize(int w, int h);

#define TEX_HEIGHT 32
#define TEX_WIDTH 32
static GLubyte image[TEX_HEIGHT][TEX_WIDTH][4];

/***********************************************************
|  �ؿ���main()
|  �������ᥤ��ؿ�
|  ������int argc       �¹Ի������ο�
|  ������char** argv    �¹Ի����������ơ�ʸ���������
|  ���͡�int            0:���ｪλ
***********************************************************/
int main(int argc, char **argv)
{
    /* ����� */
    glutInit(&argc, argv); /* OpenGL �ν���� */
    myInit(argv[0]);       /* ������ɥ�ɽ������������ν���� */
    
    Cube.ShiftX  = 0;
    Cube.ShiftY  = 0;
    Cube.ShiftZ  = 0;
    Cube.RotateX = 0;
    Cube.RotateY = 0;
    Cube.RotateZ = 0;

    glutMainLoop();

    /* �ץ���ཪλ */
    return 0;
}

void resize(int w, int h)
{
    /* ������ɥ��νĲ������׻� */
    float aspect = (float)w/ (float)h;
    /* ������ɥ����Τ�ӥ塼�ݡ��Ȥ��� */
    glViewport(0,0,w,h);

    /* CG�������� */
    glMatrixMode(GL_PROJECTION);             /* Ʃ�����(������ˡ)����⡼�ɤ��ڤ��ؤ� */
    glLoadIdentity();                        /* Ʃ����ƹ�������� */
    gluPerspective(45.0, aspect, 1.0, 20.0); /* Ʃ����ƹ�������� */
                                             /* �����45��, �Ĳ��� aspect���������̤ޤǤα��� 1.0���������̤ޤǤα��� 20.0 */
    /* ��ǥ�ӥ塼�Ѵ���������� */
    glMatrixMode(GL_MODELVIEW);
}

/**********************************************************
|  �ؿ���display()
|  �������֣���Ρץ���ե��å����襤�٥�Ƚ���
|  �������ʤ�
|  ���͡��ʤ�
***********************************************************/
void display(void)
{
    int i;

    /* ����� */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* ���̤�õ� */
    glLoadIdentity();                                   /* �������������� */

    gluLookAt(CameraX , CameraY, CameraZ, /* �����ΰ��� */
        -10, -1.0, 0.0,                         /* ������ΰ��� */
        -10.0, 1.0, 0.0);                              /* �����������Υ٥��ȥ� */


   

    //����ɽ��
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(0.0, -0.8, 0.0);
    glScalef(1000.0, 0.1, 1.1);
    glutSolidCube(1.0);
    glPopMatrix();

 /* Ω���Τ����� */
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

    /* �嵭�����褵�줿CG���˥����˽��� */
    glutSwapBuffers();
}


/***********************************************************
|  �ؿ���timer(int timerID)
|  �����������ޡ���������ַв�˥��٥�Ƚ���
|  ������int timerID    ���٥�Ȥ�ȯ�����������ޡ��μ���ID
|  ���͡��ʤ�
***********************************************************/
void timer(int timerID)
{
    int i;

    /* ���Υ����ޡ���15�ߥ��ø������ */
    glutTimerFunc(15, timer, 0);

    /* �����׵��ľ��� display() �ؿ����ƤФ��� */
    glutPostRedisplay();
    // SDL_Delay(10);
}

/***********************************************************
|  �ؿ���keyboard()
|  �����������ܡ��ɤ������줿���Υ��٥�Ƚ���
|  ������unsigned char key  �����줿������ʸ��������
|  ������int x              �����������줿�Ȥ��Υޥ����ݥ��󥿤�X��ɸ
|  ������int y              �����������줿�Ȥ��Υޥ����ݥ��󥿤�Y��ɸ
|  ���͡��ʤ�
***********************************************************/
void keyboard(unsigned char key, int x, int y)
{

    /* �����ܡ��ɽ��� */
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
    

    /* �����׵��ľ��� display() �ؿ����ƤФ��� */
    glutPostRedisplay();
}


/***********************************************************
|  �ؿ���myInit()
|  ������������ɥ�ɽ������������ν����
|  ������char *windowTitle      ������ɥ��Υ����ȥ�С���ɽ������ʸ����
|  ���͡��ʤ�
***********************************************************/
void myInit(char *windowTitle)
{
    /* OpenGL������ɥ������ޤǤν���� */
    glutInitWindowPosition(0, 0);            /* ������ɥ�ɽ������ */
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); /* ������ɥ������� */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);   /* ����⡼��  GLUT_DEPTH�Ͻ������Ť��ʤꤦ��Τ�ɬ�פʤȤ��Τ߻Ȥ� glEnable(GL_DEPTH_TEST)-- glDisable(GL_DEPTH_TEST)*/

    glutCreateWindow(windowTitle);                      /* ������ɥ���ɽ�� */
    glClearColor(0.0, 0.0, 0.0, 1.0);                   /* ���̾õ������ */

    /* ���٥��ȯ�����˸ƤӽФ��ؿ�����Ͽ */
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);  /* �����ܡ��ɤ򲡤����� */
    glutDisplayFunc(display);    /* ����ɽ�� */
    glutTimerFunc(15, timer, 0); /* �����ޡ���15�ߥ��ø������ */

    
    /*
    glEnable(GL_DEPTH_TEST);                 // ���̾õ��ͭ���ˤ���
    glEnable(GL_CULL_FACE); //3����󥰡��Ĥ���Ω�Τλ������Ф��ƥ����̤�ߤ��ʤ�����1
    glCullFace(GL_BACK);//3����󥰡��Ĥ���Ω�Τλ������Ф��ƥ����̤�ߤ��ʤ�����2
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

