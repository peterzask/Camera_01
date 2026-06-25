#ifndef CAMERA_CAL_H
#define CAMERA_CAL_H

#include "../../mainh.h"
#include "terminal_server.h"
#include <iostream>
#include <mutex>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp> /* last_measure_sections.h */
#include <opencv2/core/core.hpp>
#include <string>

struct jack_wireframe_s
{
  jack_wireframe_s()
    : Lx(0.04)
    , Wy(0.04)
    , Hz(0.04)
    , cs(0.01)
  {
    jw_rvec = (cv::Mat_<double>(3, 1) << 0, 0, 0);
    jw_tvec = (cv::Mat_<double>(4, 1) << -0.01, -0.01, 0, 1);
    build();
  }
  // modifiable: length  x, width  y, height  z, cross sections of jack
  double Lx, Wy, Hz, cs;
  // double Lx = .04, Wy = .04, Hz = .04, cs = .01;
  //  Internal value inputs
  //  Rotate and translate vectors to place jack anywhere
  //  tvec translation
  //  rvec Rodrigues rotation axis vector |rvec| = theta
  cv::Mat jw_rvec, jw_tvec;
  // Points that make up the coordinates jacks vertices
  // and the line_indx list of line end points for each line
  cv::Mat vertices, line_indx;
  cv::Mat image_vertices;
  std::vector<cv::Point> image_vertice_points;
  void build();
  void print();
  void draw(cv::Mat Perspective, cv::Mat& display_image);
};

struct perspec_s
{
  perspec_s()
    : width(2296)
    , height(4080)
    , fx(3729)
    , fy(3729)
    , u0(width / 2.0)
    , v0(height / 2.0)
    , cwR(3, 3, CV_64F, cv::Scalar(0))
    , cwk(3, 1, CV_64F, cv::Scalar(0))
    , cwt(3, 1, CV_64F, cv::Scalar(0))
    , Pext(4, 4, CV_64F, cv::Scalar(0))
    , Pint(3, 4, CV_64F, cv::Scalar(0))
    , K(3, 3, CV_64F, cv::Scalar(0))
    , Perspective(3, 4, CV_64F, cv::Scalar(0))
  {
  }
  // above magic numbers created externally from camera picture

  int width, height;     // Camera Image size
  double fx, fy, u0, v0; // Intrisic camera parameters
  cv::Mat cwR, cwk,
    cwt;           //  camera to world Rotation, K Rodrigues axis, t translate
  cv::Mat Pext;    //  [cwR,cwt;0 0 0 1] Extrinsic camera parameters
  cv::Mat Pint, K; //  Internal camera parameters
  cv::Mat Perspective; //  Perspective Transformation matrix
  void init();
  void print();
};
void perspec_copy_A2B(const struct perspec_s A, struct perspec_s& B);
void print_mat(const cv::Mat p);
void print_matRC(const cv::Mat p,int row, int col);
void fprint_mat(const cv::Mat p,const char *p_str);
struct image_data_s
{
  cv::Mat original_image, display_image;
  std::string w_name, imageFile; // = "Image 0";
  void load(void);
  void show(void);
  void refresh(void);
};
struct mouse_data_s
{
  int x, y, event, flag, new_data;
};
void onMouse(int event, int x, int y, int flag, void* userdata);
struct pan_s
{
  enum class pan_state_e
  {
    SETUP,
    P0,
    P1,
    CALC,
    SAVE_PAN,
    DONE
  };
  pan_state_e PAN_STATE = pan_state_e::SETUP;
  cv::Mat point_0, point_1, dP;
  perspec_s panMats;
  int pan(struct system_s* p, int keyStroke);
  pan_s()
    : point_0(3, 1, CV_64F)
    , point_1(3, 1, CV_64F)
    , dP(3, 1, CV_64F){};
};
struct rot_s
{
  enum class rot_state_e
  {
    SETUP,
    P0,
    MOUSE_UP,
    P1,
    MOUSE_UP2,
    ROT,
    SAVE,
    DONE
  };
  int once;
  rot_state_e ROT_STATE = rot_state_e::SETUP;
  cv::Mat point_0, point_1, point_2, V1, V2, Vr, R;
  cv::Point center;
  double x0, y0, x1, y1, rad, nv1, nv2, nvr;
  perspec_s rotMats;
  int rot(struct system_s* p, int keyStroke);
  rot_s()
    : once(0)
    , point_0(3, 1, CV_64F)
    , point_1(3, 1, CV_64F)
    , point_2(3, 1, CV_64F)
    , V1(3, 1, CV_64F)
    , V2(3, 1, CV_64F)
    , Vr(3, 1, CV_64F)
    , R(3, 3, CV_64F)
    , center(0, 0){};
};
struct rot2_s
{
  int init;
  perspec_s rot2Mats;
  cv::Point3d get_arcball_vector(int x, int y, int w, int h);
  cv::Point3d start_vec;
  int is_dragging;
  int rot2(struct system_s* p, int keyStroke);
  rot2_s():init(0), start_vec(0,0,0),is_dragging(0) 
  {};
};
struct zoomz_s
{
  enum class zoomz_state_e
  {
    SETUP,
    P0,
    MOUSE_UP,
    P1,
    CALC,
    SAVE_ZOOMZ,
    DONE
  };
  zoomz_state_e ZOOMZ_STATE = zoomz_state_e::SETUP;
  cv::Mat point_0, point_1, point_2, V1, V2;
  cv::Point center;
  perspec_s zoomzMats;
  double d1, d2, nv1, nv2;
  int zoomz(struct system_s* p, int keyStroke);
  zoomz_s()
    : point_0(3, 1, CV_64F)
    , point_1(3, 1, CV_64F)
    , point_2(3, 1, CV_64F)
    , V1(3, 1, CV_64F)
    , V2(3, 1, CV_64F)
    , center(0.0, 0.0){};
};
struct zoomf_s
{
  enum class zoomf_state_e
  {
    SETUP,
    P0,
    MOUSE_UP,
    P1,
    CALC,
    SAVE_ZOOMF,
    DONE
  };
  zoomf_state_e ZOOMF_STATE = zoomf_state_e::SETUP;
  cv::Mat point_0, point_1, point_2, V1, V2;
  cv::Point center;
  perspec_s zoomfMats;
  double d1, d2, nv1, nv2;
  int zoomf(struct system_s* p, int keyStroke);
  zoomf_s()
    : point_0(3, 1, CV_64F)
    , point_1(3, 1, CV_64F)
    , point_2(3, 1, CV_64F)
    , V1(3, 1, CV_64F)
    , V2(3, 1, CV_64F)
    , center(0.0, 0.0){};
};
struct xml_file_s
{
  int write_pMats(struct system_s* p, int keyStroke);
  int read_pMats(struct system_s* p, int keyStroke);
};
struct points_s
{
  enum class points_state_e
  {
    SETUP,
    READ_THIS,
    READ_PT,
    MOUSE_UP,
    NEXT,
    SAVE,
    DONE
  };
  points_state_e POINTS_STATE = points_state_e::SETUP;
  cv::Mat obj_points, img_points;
  int read_jack_pts(struct system_s* p, int keyStroke);
  double err;
  int indx_i, indx_j;
  points_s()
    : obj_points(17, 3, CV_64F)
    , img_points(17, 2, CV_64F){};
};
struct system_s
{
  image_data_s image;
  perspec_s pMats;
  jack_wireframe_s jack;
  mouse_data_s mouse_data, from_mouse_data, zero_mouse_data,last_mouse_data;
  int keyLast;
  std::mutex mouse_mtx;
  enum class main_state_e
  {
    PICKA,
    PICKB,
    PAN,
    ROTATE,
    ROTATE2,
    ZOOMZ,
    ZOOMF,
    DONE,
    WRITE_FILE,
    READ_FILE,
    READ_JACK_PTS
  };
  main_state_e STATE = main_state_e::PICKA;
  void init(void);
  void get_mouse_data(void);
  void print_mouse_data(int loop_cnt, int keyStroke,int keyLast);
  pan_s pan;
  rot_s rot;
  rot2_s rot2;
  zoomz_s zoomz;
  zoomf_s zoomf;
  xml_file_s xml_file;
  points_s points;
  void selectState(int keyStroke);
  system_s()
    : keyLast(0)
    , STATE(main_state_e::PICKA){};
};
void print_DLT_equations(system_s *p);
char* refresh_lines(void);
#endif // CAMERA_CAL_H

/* New function steps:
  -create struct above
  -add struct to system_s
  -add STATE name for new function to enum in system_s
  -add function to camera_cal_init.cpp
  -add banner entry to display keyStroke  used select new function
  -add function call to p->STATE switch statement in main while loop
  -add function to select switch statement

*/

/* Screen map
        1 mouse data 2 lines
        2
        3 State keys names 'q' quit 'p' pan ...
        4 Current state keys
        5 P0
        6   1
        7   2
        8   3
        9 P1
        10  1
        11  2
        12  3
        13 dU
        14  1
        15  2
        16  3


*/
