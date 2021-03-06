//---------------------------------------------------------------------------
#include <math.h>
#include <vcl.h>
#pragma hdrstop
#include <vector>

#include "main.h"
#include "Unit3.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma resource "*.dfm"
Tfmain *fmain;
//---------------------------------------------------------------------------



int i, j, k1,k2,k3, v;

  double rx, ry, rz;
  double bx, by, bz;

  double len; // vector length

  double ux, uy, uz; // plane span 1
  double nx, ny, nz; // plane span 2

  double t;
  double ax, ay, az;
  double verx, very, verz;
  double cx, cy, cz;
  
  double centre_to_inter_x, centre_to_inter_y, centre_to_inter_z;
  double xx, yy;
  double vertex_anglex, vertex_angley;
  double cosine;

  double distance_vertex_intersection;
  double distance_camera_intersection;
  double distance_vertex_camera;

  bool within_bounds;

  double sinphi, cosphi, sintheta, costheta, sineta, coseta;
  double sineta2, coseta2;

TPoint points0[4];
TPoint points1[4];
TPoint points2[4];
TPoint points3[4];
TPoint points4[4];
TPoint points5[4];

void setres(int width, int height, int colorDepth){

	TDevMode mode;

	ZeroMemory(&mode, sizeof(TDevMode));
	mode.dmSize = sizeof(TDevMode);
  	mode.dmPelsWidth = width;
  	mode.dmPelsHeight = height;
  	mode.dmBitsPerPel = colorDepth;
  	mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
  	ChangeDisplaySettings(&mode, 0);
}
void process_physics();



const MAX_BLOCKS_X = 10;
const MAX_BLOCKS_Y = 10;
const MAX_BLOCKS_Z = 10;

const double BLOCK_SIDE = 10;

class __3vertex{
	public:
    	double x, y, z;
    private:

};

class __2vertex{
	public:
    	double x, y;
       
    private:

};

class __camera{
	public:
      	double x, y, z, r;
		double phi, theta, eta;
        double vx, vy, vz;
    private:

};

class __player{
	public:
    	long int i, j, k;
        __camera _camera;
};

enum __blocktype {typeSOLID, typeWATER, typeAIR};



class __block{
	public:
    	TColor _color;
    	__blocktype _type;
        __3vertex _3vertices[8];
        __2vertex _2vertices[8];
};

__fastcall Tfmain::Tfmain(TComponent* Owner) : TForm(Owner)
{

}















  double inter_x, inter_y, inter_z;
  bool clicked;
  double height1, height2;
  __block _blocks[MAX_BLOCKS_X][MAX_BLOCKS_Y][MAX_BLOCKS_Z];
  int surface_index;

  __player _player;
  Graphics::TBitmap *screen, *final_screen;

  DWORD SCREEN_WIDTH;
  DWORD SCREEN_HEIGHT;
  double final_screen_width ;
  double final_screen_height ;
  double DISTANCE_CAMERA_SCREEN_SIDE ;
  double DISTANCE_CAMERA_SCREEN_CENTRE ;
  double ANGLE_OF_VISION ;
  double HALF_ANGLE_OF_VISION ;
  double MAX_VIEWING_DISTANCE ;
  double SPEED, ANG_SPEED ;



  double uux, uuy, uuz ;
  double nnx, nny, nnz ;

  int oldx, oldy ;
  int mouse_counter ;

  int oldwidth, oldheight ;

  double clock;

  // screen mouse variables
  int coordx, coordy ;
  double mouse_distance, mouse_angle ;
  double distance_x, distance_y ;




void read_keyboard(){

  if(GetKeyState('W') < 0){
      _player._camera.x += SPEED * cos(_player._camera.theta);
      _player._camera.y += SPEED * sin(_player._camera.theta);
      _player._camera.z += SPEED * cos(_player._camera.phi);
    }
    if(GetKeyState('S') < 0){
      _player._camera.x -= SPEED * cos(_player._camera.theta);
      _player._camera.y -= SPEED * sin(_player._camera.theta);
      _player._camera.z -= SPEED * cos(_player._camera.phi);
    }
    if (GetKeyState(VK_DOWN) < 0) {
      //if camera.phi + 0.01 <= pi then
      if(_player._camera.phi + ANG_SPEED < M_PI)
        _player._camera.phi = _player._camera.phi + ANG_SPEED;
    }
    if (GetKeyState(VK_UP) < 0) 
    {
      //if camera.phi - 0.01 >= 0 then

      if(_player._camera.phi - ANG_SPEED > 0)
        _player._camera.phi = _player._camera.phi - ANG_SPEED;
    }
    if(GetKeyState(VK_SHIFT) < 0){
    
      if(GetKeyState(VK_LEFT) < 0){
        _player._camera.eta = _player._camera.eta - ANG_SPEED;
      }
      if(GetKeyState(VK_RIGHT) < 0){

        _player._camera.eta = _player._camera.eta + ANG_SPEED;
      }
    }
    else{
      if(GetKeyState(VK_LEFT) < 0)
        _player._camera.theta = _player._camera.theta - ANG_SPEED;
      if(GetKeyState(VK_RIGHT) < 0){
        _player._camera.theta = _player._camera.theta + ANG_SPEED;
		}
    }

    if (GetKeyState('A') < 0){
      _player._camera.x = _player._camera.x - SPEED * nnx;
      _player._camera.y = _player._camera.y - SPEED * nny;
      _player._camera.z = _player._camera.z - SPEED * nnz;
	}
    if(GetKeyState('D') < 0){
      _player._camera.x = _player._camera.x + SPEED * nnx;
      _player._camera.y = _player._camera.y + SPEED * nny;
      _player._camera.z = _player._camera.z + SPEED * nnz;
    }
  
    if(GetKeyState('Q') < 0){
      _player._camera.x = _player._camera.x - SPEED * uux;
      _player._camera.y = _player._camera.y - SPEED * uuy;
      _player._camera.z = _player._camera.z - SPEED * uuz;
    }
    if(GetKeyState('E') < 0){
      _player._camera.x = _player._camera.x + SPEED * uux;
      _player._camera.y = _player._camera.y + SPEED * uuy;
      _player._camera.z = _player._camera.z + SPEED * uuz;
    }


}

//---------------------------------------------------------------------------



void __fastcall Tfmain::FormCreate(TObject *Sender)
{

  int i, j, k;
  double x, y, z;

  DoubleBuffered = true;

  screen = new Graphics::TBitmap();
  screen->Width = fmain->pscreen->Width;
  screen->Height = pscreen->Height;
  screen->Canvas->Pen->Color = clBlack;
  screen->Canvas->Brush->Color = clTeal;

  final_screen = new Graphics::TBitmap();
  final_screen->Canvas->Pen->Color = clBlack;
  final_screen->Canvas->Brush->Color = clTeal;

  _player._camera.phi = M_PI/2.0;
  _player._camera.theta = M_PI/4.0;
  _player._camera.y = 1.0;
  _player._camera.x = 1.0;
  _player._camera.z = 10.0;


  randomize;
  for(i = 0; i < MAX_BLOCKS_X; i++){
  	for(j = 0; j < MAX_BLOCKS_Y; j++){
    	for(k = 0; k < MAX_BLOCKS_Z; k++){
        	x = random(10);
            y = random(10);
            z = random(10);
        	//if(k > 0) _blocks[i][j][k]._type = typeAIR;
    		//else
            _blocks[i][j][k]._type = typeSOLID;
    		_blocks[i][j][k]._color = RGB(random(255), random(255), random(255));

  	_blocks[i][j][k]._3vertices[0].x = i * x*10*BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[0].y = j * y*10*BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[0].z = k * z*10*BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[1].x = i * x*10*BLOCK_SIDE + BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[1].y = j * y*10*BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[1].z = k * z*10*BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[2].x = i * x*10*BLOCK_SIDE + BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[2].y = j * y*10*BLOCK_SIDE + BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[2].z = k * z*10*BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[3].x = i * x*10*BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[3].y = j * y*10*BLOCK_SIDE + BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[3].z = k * z*10*BLOCK_SIDE;

    _blocks[i][j][k]._3vertices[4].x = i * x*10*BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[4].y = j * y*10*BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[4].z = k * z*10*BLOCK_SIDE + BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[5].x = i * x*10*BLOCK_SIDE + BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[5].y = j * y*10*BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[5].z = k * z*10*BLOCK_SIDE + BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[6].x = i * x*10*BLOCK_SIDE + BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[6].y = j * y*10*BLOCK_SIDE + BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[6].z = k * z*10*BLOCK_SIDE + BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[7].x = i * x*10*BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[7].y = j * y*10*BLOCK_SIDE + BLOCK_SIDE;
    _blocks[i][j][k]._3vertices[7].z = k * z*10*BLOCK_SIDE + BLOCK_SIDE;
    


    }
    }
  }


  SPEED = 2;
  ANG_SPEED = 0.025;
  DISTANCE_CAMERA_SCREEN_CENTRE = 1.0;
  ANGLE_OF_VISION = M_PI / 2.0;
  HALF_ANGLE_OF_VISION = ANGLE_OF_VISION / 2.0;
  MAX_VIEWING_DISTANCE = 100000.0;
  SCREEN_WIDTH = 2.0 * DISTANCE_CAMERA_SCREEN_CENTRE;
  SCREEN_HEIGHT = 2.0 * DISTANCE_CAMERA_SCREEN_CENTRE;
  DISTANCE_CAMERA_SCREEN_SIDE = DISTANCE_CAMERA_SCREEN_CENTRE / 0.70710678;
}

double length(double x, double y, double z){
	return sqrt(x*x + y*y + z*z);
}


double dot(double a, double b, double c, double x, double y, double z){
	return a * x + b * y + c * z;
}




//---------------------------------------------------------------------------


void __fastcall Tfmain::Exit1Click(TObject *Sender)
{
	Application->Terminate();	
}
//---------------------------------------------------------------------------


void draw_perspective(){
  

  sinphi = sin(_player._camera.phi);
  cosphi = cos(_player._camera.phi);
  sintheta = sin(_player._camera.theta);
  costheta = cos(_player._camera.theta);
  sineta = sin(_player._camera.eta);
  coseta = cos(_player._camera.eta);
  sineta2 = sin(_player._camera.eta + M_PI/2.0);
  coseta2 = cos(_player._camera.eta + M_PI/2.0);

//stretchblt(screen->canvas->Handle, 0,0,screen->width,screen->Height, fmain->image1->Canvas->Handle,0,0,fmain->Image1->width,fmain->Image1->Height, srccopy);
//screen->canvas->Draw(0,0,fmain->image1->Picture->Bitmap);
  screen->Canvas->FillRect(Rect(0, 0, screen->Width, screen->Height));

  // lets find the vision angle vector
  _player._camera.vx = DISTANCE_CAMERA_SCREEN_CENTRE * sinphi * costheta;
  _player._camera.vy = DISTANCE_CAMERA_SCREEN_CENTRE * sinphi * sintheta;
  _player._camera.vz = DISTANCE_CAMERA_SCREEN_CENTRE * cosphi;

  // now lets calculate the first perpendicular vector (u) by doing a dot product with the view vector
  // for this vector we want the Z component to be 0 so that it is parallel to the ground->
  // the Y component can be anything, and then we find the X component
  // we will use this vector and the one perpendicular to it as a building set for the actual vectors we will use
  // as the vectors we use to calculate distances inside the vision plane
  uz = 0.0;
  if(cos(_player._camera.theta) >= 0.0){
    uy = 1.0;
   ux = -_player._camera.vy/_player._camera.vx;

  }
  else{
    uy = -1.0;
    ux = _player._camera.vy/_player._camera.vx;

  }
  // and convert it into a unit vector
  len = sqrt(ux*ux + uy*uy + uz*uz);
  ux = ux / len;
  uy = uy / len;
  uz = uz / len;

  // now calculate the second perpendicular to the view vector by taking the cross product
  nx = uz*_player._camera.vy - uy*_player._camera.vz;
  ny = ux*_player._camera.vz - uz*_player._camera.vx;
  nz = uy*_player._camera.vx - ux*_player._camera.vy;
  // and convert it into a unit vector
  len = sqrt(nx*nx + ny*ny + nz*nz);
  nx = nx/len;
  ny = ny/len;
  nz = nz/len;

  // we now build the actual vectors we use inside the plane by using the previous two vectors as building vectors
  // we do this so that we can freely choose 2 vectors based on the angle eta->
  uux = ux * coseta + nx * sineta;
  uuy = uy * coseta + ny * sineta;
  uuz = uz * coseta + nz * sineta;
  nnx = ux * coseta2 + nx * sineta2;
  nny = uy * coseta2 + ny * sineta2;
  nnz = uz * coseta2 + nz * sineta2;

  // camera coordinates
  cx = _player._camera.x;
  cy = _player._camera.y;
  cz = _player._camera.z;

  // vision plane centre
  ax = cx + _player._camera.vx;
  ay = cy + _player._camera.vy;
  az = cz + _player._camera.vz;

  // now we find the intersection of each vertex with the vision plane
    for(k1= 0; k1 < MAX_BLOCKS_X; k1++){
        for(k2 = 0; k2 < MAX_BLOCKS_Y; k2++){
        	for(k3 = 0; k3 < MAX_BLOCKS_Z; k3++){
            	within_bounds = true;
                for(v = 0; v < 8; v++){ // block surfaces
                  // current vertex coordinates
                    verx = _blocks[k1][k2][k3]._3vertices[v].x;
                    very = _blocks[k1][k2][k3]._3vertices[v].y;
                    verz = _blocks[k1][k2][k3]._3vertices[v].z;
                    // parameter for line from camera to current vertex
                    // if(( (verx - cx) * sin(camera.phi)*cos(camera.theta) + (very - cy)*sin(camera.phi)*sin(camera.theta) + (verz - cz)*cos(camera.phi) ) != 0.0)
                    t = ( _player._camera.vx*sinphi*costheta + _player._camera.vy*sinphi*sintheta + _player._camera.vz*cosphi )
                        / ( (verx - cx) * sinphi*costheta + (very - cy)*sinphi*sintheta + (verz - cz)*cosphi );
                    // this is the intersection point on the plane
                    inter_x = cx + (verx - cx) * t;
                    inter_y = cy + (very - cy) * t;
                    inter_z = cz + (verz - cz) * t;
                    // this is a vector from the plane centre to the intersection point on the plane
                    centre_to_inter_x = inter_x - ax;
                    centre_to_inter_y = inter_y - ay;
                    centre_to_inter_z = inter_z - az;

                    // distances
                    distance_vertex_intersection = sqrt((verx - inter_x)*(verx - inter_x) + (very - inter_y)*(very - inter_y) + (verz - inter_z)*(verz - inter_z));
                    distance_camera_intersection = sqrt((inter_x - cx)*(inter_x - cx) + (inter_y - cy)*(inter_y - cy) + (inter_z - cz)*(inter_z - cz));
                    distance_vertex_camera = sqrt((verx - cx)*(verx - cx) + (very - cy)*(very - cy) + (verz - cz)*(verz - cz));
                    // cosine between viewing vector and vertex vector to check if the vertex is within field of view (a circle)  (from dot product formula)
                    cosine = (_player._camera.vx*(verx-cx) + _player._camera.vy*(very-cy) + _player._camera.vz*(verz-cz))
                    / sqrt(_player._camera.vx*_player._camera.vx + _player._camera.vy*_player._camera.vy + _player._camera.vz*_player._camera.vz)
                    / sqrt((verx-cx)*(verx-cx) + (very-cy)*(very-cy) + (verz-cz)*(verz-cz));


                    // if any of the vertices are outside the frustrum, we quit the testing of vertices for this cube
                    if(( distance_vertex_camera < distance_camera_intersection)
                    	|| (distance_vertex_intersection > MAX_VIEWING_DISTANCE)
                    	|| ( fabs(acos(cosine)) > M_PI/2.0)){
                    	within_bounds = false;
                    	break;
                    }

                    // now we need to find the lengths of both components of the inside vector
                    // and check if they are within the bounds of the plane
                    // to do this we take the dot product between the unit vectors which span the vision plane
                    //and the vectors from the plane centre to the intersection
                    xx = centre_to_inter_x*uux + centre_to_inter_y*uuy + centre_to_inter_z*uuz;
                    yy = centre_to_inter_x*nnx + centre_to_inter_y*nny + centre_to_inter_z*nnz;
                    _blocks[k1][k2][k3]._2vertices[v].x = xx / (SCREEN_WIDTH / 2.0);
                    _blocks[k1][k2][k3]._2vertices[v].y = yy / (SCREEN_HEIGHT / 2.0);
                }
        		// if outside bounds, quit the plotting for this cube
                if(within_bounds){
                    points0[0] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[0].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[0].y));
                    points0[1] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[1].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[1].y));
                    points0[2] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[2].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[2].y));
                    points0[3] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[3].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[3].y));

                    points1[0] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[4].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[4].y));
                    points1[1] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[5].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[5].y));
                    points1[2] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[6].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[6].y));
                    points1[3] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[7].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[7].y));

                    points2[0] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[0].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[0].y));
                    points2[1] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[3].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[3].y));
                    points2[2] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[7].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[7].y));
                    points2[3] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[4].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[4].y));

                    points3[0] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[1].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[1].y));
                    points3[1] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[2].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[2].y));
                    points3[2] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[6].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[6].y));
                    points3[3] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[5].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[5].y));

                    points4[0] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[0].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[0].y));
                    points4[1] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[1].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[1].y));
                    points4[2] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[5].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[5].y));
                    points4[3] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[4].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[4].y));

                    points5[0] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[3].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[3].y));
                    points5[1] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[2].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[2].y));
                    points5[2] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[6].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[6].y));
                    points5[3] = Point(screen->Width / 2 + ((screen->Width/2) * _blocks[k1][k2][k3]._2vertices[7].x), screen->Height / 2 - ( (screen->Height/2) * _blocks[k1][k2][k3]._2vertices[7].y));

                    screen->Canvas->Brush->Color = _blocks[k1][k2][k3]._color;
                    screen->Canvas->Pen->Color = _blocks[k1][k2][k3]._color;
                    screen->Canvas->Polygon(points0, 3);
                    screen->Canvas->Polygon(points1, 3);
                    screen->Canvas->Polygon(points2, 3);
                    screen->Canvas->Polygon(points3, 3);
                    screen->Canvas->Polygon(points4, 3);
                    screen->Canvas->Polygon(points5, 3);
                }
            }
        }
    }
       screen->Canvas->Brush->Color = clWhite;          /*
  // draw screen cross
  screen->Canvas->Pen->Color = RGB(100, 100, 100);
  screen->Canvas->MoveTo(screen->Width / 2 - 10, screen->Height / 2);
  screen->Canvas->LineTo(screen->Width / 2 + 10, screen->Height / 2);
  screen->Canvas->MoveTo(screen->Width / 2, screen->Height / 2 - 10);
  screen->Canvas->LineTo(screen->Width / 2, screen->Height / 2 + 10);



  // parameter for line
  rx = uux * SCREEN_WIDTH * (coordx - screen->Width / 2.0) / screen->Width -
    nnx * SCREEN_HEIGHT * (coordy - screen->Height / 2.0) / screen->Height;
  ry = uuy * SCREEN_WIDTH * (coordx - screen->Width / 2.0) / screen->Width -
    nny * SCREEN_HEIGHT * (coordy - screen->Height / 2.0) / screen->Height;
  rz = uuz * SCREEN_WIDTH * (coordx - screen->Width / 2.0) / screen->Width -
    nnz * SCREEN_HEIGHT * (coordy - screen->Height / 2.0) / screen->Height;

  rx = rx + cx + _player._camera.vx;
  ry = ry + cy + _player._camera.vy;
  rz = rz + cz + _player._camera.vz;

  //vector from camera to plane intersection    s
  // c = camera
  // a = intersection - camera 
  ax = rx - cx;
  ay = ry - cy;
  az = rz - cz;

  if((cz - rz) > 0.0)
  	t = cz / (cz - rz);
  else t = 0.0;

  // this is the intersection point on the ground plane
  inter_x = cx + ax * t;
  inter_y = cy + ay * t;
  inter_z = cz + az * t;

                      
                   */


}



void __fastcall Tfmain::FormActivate(TObject *Sender)
{
	int i, j, k;
  double x, y, z;

  oldheight = screen->Height;
  oldwidth = screen->Width;
	//SetScreenResolution(640, 480, 8);

    
  while(!Application->Terminated){
    clock = clock + 0.1;
    read_keyboard();

    //animate();
    //process_physics();
    //if(CheckBox2->Checked)

    draw_perspective();
    //update_info();
                         
    fmain->pscreen->Canvas->Draw(0, 0, screen);

    process_physics();

    

    
    Application->ProcessMessages();
  }
}

const double GRAVITY = 9.8;

void process_physics(){
	_player.i = _player._camera.x / BLOCK_SIDE;
    _player.j = _player._camera.y / BLOCK_SIDE;
    _player.k = _player._camera.z / BLOCK_SIDE;

    fmain->edtplayer->Text = IntToStr(_player.i) + ":" + IntToStr(_player.j) + ":" + IntToStr(_player.k);
	
	
};

//---------------------------------------------------------------------------

