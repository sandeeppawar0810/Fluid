#include "GridStag.h"

#include "Renderer.h"
#include "FluidSim.h"
#include "keyboard.h"
#include "Printer.h"
#include "Plotter.h"
#include "opengl.h"
#include <string>

GridStag* sGrid = new GridStag ;
//GridStag* sGrid = new GridStag ;

Renderer* render = new Renderer ;
Printer * print = new Printer ;
FluidSim* fluidSim = new FluidSim ;
Plotter* plotr = new Plotter ;

int winId;
unsigned int winSizeX = 600;
unsigned int winSizeY = 600;
int  omx, omy, mouseButton[3];
double mx,my;

double timestep = 0.01;

void display ( void ) ;
void init ( void ) ;
void animate () ;


void mouseFun ( int button, int state, int x, int y ) ;
void motionFun ( int x, int y ) ;	
void preDisplay ( );
void postDisplay ( );
void openGlutWindow ( char* windowName) ;
void reshape ( int w, int h ) ;

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   init ();
   char windowName[] = "   Liquid_Simulator- tryLevelSet" ;
   openGlutWindow(windowName);
   glutMainLoop();
   return 0;
}

//---------glut and opengl.....Initializations
void animate()
{
	fluidSim->simulate(timestep);

	extern int flagC;
//	static int it = 0;

	#define MAX_ITERATION 1800
	if(flagC==2){
	//	it=0;
		flagC=10;
	}
	//cout<< ++it <<endl ;
} 

void init(void) 
{
   extern int flagC ;
   flagC = 0;
   sGrid->initGridStag();
   render->init(sGrid);
   print->init(sGrid);
   fluidSim->init(sGrid);
   fluidSim->initFluidBody(fBT);// 1: indicates dam break rhb
   plotr->init(sGrid,200,26,MAX_ITERATION);//number of intervals,noofFiles,maxIteration
}

void initParticles()
{
	sGrid->fluidParticles.clear();
}

void display(void)
{
   preDisplay();
   switch(output)
   {
		case VEL :
			render->renderVector2D(sGrid->u,sGrid->v);
			break;
		case DEN :
			render->renderDen2D_Stam();	
			break;
		case PAR :
			render->renderParticles();
			break;
		case DUB :
			render->renderVector2D(sGrid->u,sGrid->v);
			render->renderParticles();
			break;
		case PRE :
			//render->renderMat(sGrid->p,2);
			render->renderMat(sGrid->cellType,1);
			break;	
   }	   
	if(AdvectFlagCell || AdvectFlagParticle){
		render->renderParticle(traceAdvectCellsd[2],traceAdvectCellsd[3], 0.6,0,0,12);//R G B Size
		render->renderParticle(traceAdvectCellsd[0],traceAdvectCellsd[1], 0,1,0,8);//R G B Size
	}
	//capture_frame(framenum++);
	//cout<<framenum++<<endl;
	postDisplay();
}

void idleFun ( void )
{
	 animate();
	 glutSetWindow ( winId );
	 glutPostRedisplay ( );
}

void motionFun ( int x, int y )
{
	mx = x;
	my = y;
}

void processMouseInCoords(double mx, double my)
{
	cout<<endl<<"Clicked At     : "<<mx<<" "<<my<<endl;
	if(AdvectFlagCell){
		fluidSim->advect2DCell((int)mx,(int)my,fluidSim->dt,sGrid->u,sGrid->v,2);
	}
	else if(AdvectFlagParticle){
		fluidSim->advect2DParticle(mx, my,fluidSim->dt,sGrid->u,sGrid->v);
	}
}
void mouseFun ( int button, int state, int x, int y )
{
	mouseButton[button] = ( state == GLUT_DOWN ) ;	
	if(mouseButton[0]){
	mx = ( (x/(double)winSizeX) * (sGrid->nX) ) ;
	my = ( ((winSizeY - y) / (double)winSizeY) * ( sGrid->nY) ); //not covers exact cell..
	
	processMouseInCoords(mx,my);
	}
}

void reshape (int w, int h)
{
   glutSetWindow(winId);
   glutReshapeWindow(w ,h);
   winSizeX = w;
   winSizeY = h;
}

void preDisplay()
{
	glViewport ( 0, 0, winSizeX, winSizeY);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho( 0, zoomFactor, 0, zoomFactor	, 0,1 ); //better to use ortho..
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void postDisplay()
{
   glutSwapBuffers();
}	

void openGlutWindow(char* windowName)
{
   glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE );
   glutInitWindowSize (winSizeX, winSizeY); 
   glutInitWindowPosition (570, 100);
 
   winId = glutCreateWindow (&windowName[2]);

   glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);
   glutSwapBuffers();
   glClear(GL_COLOR_BUFFER_BIT);
   glutSwapBuffers();
   createMenu();
   glutDisplayFunc(display); 
   glutSpecialFunc(&SpecialKeyPressed);
   glutKeyboardFunc(&KeyPressed);
   glutReshapeFunc(reshape);
   glutMouseFunc(mouseFun);
   glutMotionFunc(motionFun);
   glutIdleFunc(idleFun);
}


void menu(int num){
  if(num == 27){
    glutDestroyWindow(winId);
    exit(0);
  }
  switch(num)
  {
	case 0:	
	case 1:	
	case 2:
			ProjectFLAG = num  ;
			break;
	case 3:	
	case 4:	
	case 5:
	case 6:
			output = (renderFlag)num;
			break;
  }
  glutPostRedisplay();
}
