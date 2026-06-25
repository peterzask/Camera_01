#include "camera_cal.h"
#include <unistd.h>
#ifdef INIT_MAIN_TEST
int
main(int argc, char** argv)
{


  struct system_s system_base,*p=&system_base;
  p->image.load();
  p->image.show();
  p->pMats.init();
  // p->pMats.print();
  // p->jack.print();
  p->jack.draw(p->pMats.Perspective, p->image.display_image);
  p->image.show();
  cv::setMouseCallback(p->image.w_name, onMouse, p);
  clr();
  int keyStroke = 0, loop_cnt = 0, done = 0;
  p->STATE = p->main_state_e::PICKA;
  while (0 == done)
  {
    keyStroke = cv::waitKey(100);
    ++loop_cnt;
    p->get_mouse_data();
    if (1 == p->from_mouse_data.new_data || keyStroke != -1)
    { 
      p->print_mouse_data(loop_cnt, keyStroke,p->keyLast); 
      if (keyStroke != -1) 
      p->keyLast = keyStroke; 
    }
    if (p->STATE == p->main_state_e::PICKA)
    {
      rc(3, 0);
      printf("State:'q'-quit 'p'-pan 'r'-rotate '2' -rot2 'z'-zoom"
             " 'f'-zoomf  'w' - write_pMats  '4' - read_pMats "
             " '6' read_jack_pts          ");
      fflush(stdout);
      p->STATE = p->main_state_e::PICKB;
    }
    if (1 == p->from_mouse_data.new_data || keyStroke != -1)
    switch (p->STATE)
    {
      case p->main_state_e::PICKB:
      {
        p->selectState(keyStroke);
      }
      break;
      case p->main_state_e::PAN:
      {
        if (1 == p->pan.pan(p, keyStroke))
        {
          p->STATE = p->main_state_e::PICKA;
        }
      }
      break;
      case p->main_state_e::ROTATE:
      {
        if (1 == p->rot.rot(p, keyStroke))
        {
          p->STATE = p->main_state_e::PICKA;
        }
      }
      break;
      case p->main_state_e::ROTATE2:
      {
        if (1 == p->rot2.rot2(p, keyStroke))
        {
          p->STATE = p->main_state_e::PICKA;
        }
      }
      break;
      case p->main_state_e::ZOOMZ:
      {
        if (1 == p->zoomz.zoomz(p, keyStroke))
        {
          p->STATE = p->main_state_e::PICKA;
        }
      }
      break;
      case p->main_state_e::ZOOMF:
      {
        if (1 == p->zoomf.zoomf(p, keyStroke))
        {
          p->STATE = p->main_state_e::PICKA;
        }
      }
      break;
      case p->main_state_e::DONE:
      {
        done = 1;
        rc(1, 0);
        refresh_lines();
        rc(1, 0);
      }
      break;
      case p->main_state_e::WRITE_FILE:
      {
        if (1 == p->xml_file.write_pMats(p, keyStroke))
        {
          p->STATE = p->main_state_e::PICKA;
        }
      }
      break;
      case p->main_state_e::READ_FILE:
      {
        if (1 == p->xml_file.read_pMats(p, keyStroke))
        {
          p->STATE = p->main_state_e::PICKA;
        }
      }
      break;
      case p->main_state_e::READ_JACK_PTS:
      {
        if (1 == p->points.read_jack_pts(p, keyStroke))
        {
          p->STATE = p->main_state_e::PICKA;
        }
      }
      break;
      default:
        break;
    }
    if('q' == keyStroke) break;

  }
  return 0;
}
#endif // INIT_MAIN_TEST
void
system_s::selectState(int keyStroke)
{
  switch (keyStroke)
  {
    case 'q':
      STATE = main_state_e::DONE;
      break;
    case 'p':
      STATE = main_state_e::PAN;
      break;
    case 'r':
      STATE = main_state_e::ROTATE;
      break;
    case 'z':
      STATE = main_state_e::ZOOMZ;
      break;
    case 'f':
      STATE = main_state_e::ZOOMF;
      break;
    case 'w':
      STATE = main_state_e::WRITE_FILE;
      break;
    case '4':
      STATE = main_state_e::READ_FILE;
      break;
    case '6':
      STATE = main_state_e::READ_JACK_PTS;
      break;
    case '2':
      STATE = main_state_e::ROTATE2;
      break;
    default:
      break; // STATE is unchanged
  }
}
int
xml_file_s::write_pMats(struct system_s* p, int keyStroke)
{
  cv::FileStorage fs("pMats.xml", cv::FileStorage::WRITE);

  time_t rawtime;
  time(&rawtime);
  fs << "calibrationDate" << asctime(localtime(&rawtime));
  fs << "ImageFilename" << p->image.imageFile;
  fs << "width" << p->pMats.width << "height" << p->pMats.height << "fx"
     << p->pMats.fx << "fy" << p->pMats.fy;
  fs << "u0" << p->pMats.u0 << "v0" << p->pMats.v0;
  fs << "cwR" << p->pMats.cwR << "cwk" << p->pMats.cwk << "cwt" << p->pMats.cwt;
  fs << "Pext" << p->pMats.Pext << "Pint" << p->pMats.Pint << "K" << p->pMats.K;
  fs << "Perspective" << p->pMats.Perspective;

  fs.release();
  return 1;
}

int
xml_file_s::read_pMats(struct system_s* p, int keyStroke)
{
  cv::FileStorage fs2("pMats.xml", cv::FileStorage::READ);
  fs2["width"] >> p->pMats.width;
  fs2["height"] >> p->pMats.height;
  fs2["fx"] >> p->pMats.fx;
  fs2["fy"] >> p->pMats.fy;
  fs2["u0"] >> p->pMats.u0;
  fs2["v0"] >> p->pMats.v0;
  fs2["cwR"] >> p->pMats.cwR;
  fs2["cwk"] >> p->pMats.cwk;
  fs2["cwt"] >> p->pMats.cwt;
  fs2["Pext"] >> p->pMats.Pext;
  fs2["Pint"] >> p->pMats.Pint;
  fs2["K"] >> p->pMats.K;
  fs2["Perspective"] >> p->pMats.Perspective;
  fs2.release();
  return 1;
}

int
zoomf_s::zoomf(struct system_s* p, int keyStroke)
{
  static int once = 0; // State for banner printing
  switch (ZOOMF_STATE)
  {
    case zoomf_state_e::SETUP:
    {
      rc(4, 0);
      printf(
        "ZOOMZ STATE 's' to save zoomz, 'esc' to leave no save.          ");
      fflush(stdout);

      perspec_copy_A2B(p->pMats, zoomfMats);
      ZOOMF_STATE = zoomf_state_e::P0;
    }
    break;
    case zoomf_state_e::P0:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ZOOMF STATE P0  's' to save zoomf, 'esc' to leave no save.     "
               "     ");
        fflush(stdout);
      }
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        // Middle mouse button down, Store rot start point in point_0
        point_0.at<double>(0, 0) = center.x = p->from_mouse_data.x;
        point_0.at<double>(1, 0) = center.y = p->from_mouse_data.y;
        point_0.at<double>(2, 0) = 0;
        cv::circle(p->image.display_image, center, 40, cv::Scalar(0, 0, 255),
                   2);
        p->image.show();
        rc(5, 0);
        puts("P0 ");
        print_mat(point_0);
        ZOOMF_STATE = zoomf_state_e::MOUSE_UP;
        once = 0;
      }
    }
    break;
    case zoomf_state_e::MOUSE_UP:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ZOOMF STATE MOUSE_UP  's' to save, 'esc' to leave.          ");
        fflush(stdout);
      }
      // Wait for center mouse button up
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        ZOOMF_STATE = zoomf_state_e::P1;
        once = 0;
      }
    }
    break;
    case zoomf_state_e::P1:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ZOOMF STATE P1      's' to save, 'esc' to leave         ");
        fflush(stdout);
      }
      // center mouse button down, store point_1
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        // Store rot start point in point_0
        point_1.at<double>(0, 0) =
          p->from_mouse_data.x; // center.x = p->from_mouse_data.x;
        point_1.at<double>(1, 0) =
          p->from_mouse_data.y; // center.y = p->from_mouse_data.y;
        point_1.at<double>(2, 0) = 0;
        d1 = cv::norm(point_0, point_1, cv::NORM_L2);
        cv::circle(p->image.display_image, center, (int)d1,
                   cv::Scalar(0, 0, 255), 2);
        p->image.show();
        rc(9, 0);
        puts("P1             ");
        print_mat(point_1);
        ZOOMF_STATE = zoomf_state_e::CALC;
        once = 0;
      }
    }
    break;
    case zoomf_state_e::CALC:
    {
      // middle mouse button down:
      // calculate rotation vector =
      // v = cross( normalized(p2-p0),zVector)*(1-|p2-p0]/|p1-p0|)
      // cwR = Rotv(v) * cwR
      // P = Pint * [cwR,cwt;[0 0 0 1]]
      // refresh
      // draw circle 0, draw circle 1 on display_image
      // jackDraw(P,display_image
      // show, next round until mouse up
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ZOOMF CALC                                       ");
        fflush(stdout);
      }
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        point_2.at<double>(0, 0) =
          p->from_mouse_data.x; // center.x = p->from_mouse_data.x;
        point_2.at<double>(1, 0) =
          p->from_mouse_data.y; // center.y = p->from_mouse_data.y;
        point_2.at<double>(2, 0) = 0;
        V1 = point_1 - point_0;
        nv1 = cv::norm(V1, cv::NORM_L2);
        V2 = point_2 - point_0;
        nv2 = cv::norm(V2, cv::NORM_L2);
        double Scale = nv2 / nv1;
        Scale = pow(Scale, 3);
        zoomfMats.fx = Scale * p->pMats.fx;
        zoomfMats.fy = Scale * p->pMats.fy;
        zoomfMats.Pint =
          (cv::Mat_<double>(3, 4) << zoomfMats.fx, 0, zoomfMats.u0, 0, 0,
           zoomfMats.fy, zoomfMats.v0, 0, 0, 0, 1, 0);
        zoomfMats.Perspective = zoomfMats.Pint * zoomfMats.Pext;
        p->image.refresh();
        p->jack.draw(zoomfMats.Perspective, p->image.display_image);
        cv::circle(p->image.display_image, center, 40, cv::Scalar(0, 0, 255),
                   2);
        cv::circle(p->image.display_image, center, nv1, cv::Scalar(0, 0, 255),
                   2);
        p->image.show();

        rc(13, 0);
        puts("zoomfPmats.Pint");
        print_mat(zoomfMats.Pint);
        printf("(Scale)(%10.4f) nv2(%10.4f),nv1(%10.4f)               ", Scale,
               nv2, nv1);
        fflush(stdout);
      }
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        ZOOMF_STATE = zoomf_state_e::SAVE_ZOOMF;
        p->keyLast = -1;
        once = 0;
      }
    }
    break;
    case zoomf_state_e::SAVE_ZOOMF:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ZOOMF STATE SAVE_ZOOMF      's' to save, 'esc' to leave    ");
      }
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        if ('s' == p->keyLast)
        {
          perspec_copy_A2B(zoomfMats, p->pMats);
          ZOOMF_STATE = zoomf_state_e::DONE;
          p->keyLast = -1;
          once = 0;
        }
        else if (27 == p->keyLast || ' ' == p->keyLast)
        {
          ZOOMF_STATE = zoomf_state_e::DONE;
          p->keyLast = -1;
          once = 0;
        }
      }
    }
    break;
    case zoomf_state_e::DONE:
    {
      rc(4, 0);
      puts("Zoomf done");
      refresh_lines();
      p->image.refresh();
      p->jack.draw(p->pMats.Perspective, p->image.display_image);
      p->image.show();
      ZOOMF_STATE = zoomf_state_e::SETUP;
      once = 0;
      return 1;
    }
    break;
    default:
      break;
  }
  return 0;
}

int
zoomz_s::zoomz(struct system_s* p, int keyStroke)
{
  static int once = 0; // State for banner printing
  switch (ZOOMZ_STATE)
  {
    case zoomz_state_e::SETUP:
    {
      rc(4, 0);
      printf(
        "ZOOMZ STATE 's' to save zoomz, 'esc' to leave no save.          ");
      fflush(stdout);

      perspec_copy_A2B(p->pMats, zoomzMats);
      ZOOMZ_STATE = zoomz_state_e::P0;
    }
    break;
    case zoomz_state_e::P0:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ZOOMZ STATE P0  's' to save zoomz, 'esc' to leave no save.     "
               "     ");
        fflush(stdout);
      }
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        // Middle mouse button down, Store rot start point in point_0
        point_0.at<double>(0, 0) = center.x = p->from_mouse_data.x;
        point_0.at<double>(1, 0) = center.y = p->from_mouse_data.y;
        point_0.at<double>(2, 0) = 0;
        cv::circle(p->image.display_image, center, 40, cv::Scalar(0, 0, 255),
                   2);
        p->image.show();
        rc(5, 0);
        puts("P0 ");
        print_mat(point_0);
        ZOOMZ_STATE = zoomz_state_e::MOUSE_UP;
        once = 0;
      }
    }
    break;
    case zoomz_state_e::MOUSE_UP:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ZOOMZ STATE MOUSE_UP  's' to save, 'esc' to leave.          ");
        fflush(stdout);
      }
      // Wait for center mouse button up
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        ZOOMZ_STATE = zoomz_state_e::P1;
        once = 0;
      }
    }
    break;
    case zoomz_state_e::P1:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ZOOMZ STATE P1      's' to save, 'esc' to leave         ");
        fflush(stdout);
      }
      // center mouse button down, store point_1
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        // Store rot start point in point_0
        point_1.at<double>(0, 0) =
          p->from_mouse_data.x; // center.x = p->from_mouse_data.x;
        point_1.at<double>(1, 0) =
          p->from_mouse_data.y; // center.y = p->from_mouse_data.y;
        point_1.at<double>(2, 0) = 0;
        d1 = cv::norm(point_0, point_1, cv::NORM_L2);
        cv::circle(p->image.display_image, center, (int)d1,
                   cv::Scalar(0, 0, 255), 2);
        p->image.show();
        rc(9, 0);
        puts("P1             ");
        print_mat(point_1);
        ZOOMZ_STATE = zoomz_state_e::CALC;
        once = 0;
      }
    }
    break;
    case zoomz_state_e::CALC:
    {
      // middle mouse button down:
      // calculate rotation vector =
      // v = cross( normalized(p2-p0),zVector)*(1-|p2-p0]/|p1-p0|)
      // cwR = Rotv(v) * cwR
      // P = Pint * [cwR,cwt;[0 0 0 1]]
      // refresh
      // draw circle 0, draw circle 1 on display_image
      // jackDraw(P,display_image
      // show, next round until mouse up
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ZOOMZ CALC                                       ");
        fflush(stdout);
      }
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        point_2.at<double>(0, 0) =
          p->from_mouse_data.x; // center.x = p->from_mouse_data.x;
        point_2.at<double>(1, 0) =
          p->from_mouse_data.y; // center.y = p->from_mouse_data.y;
        point_2.at<double>(2, 0) = 0;
        V1 = point_1 - point_0;
        nv1 = cv::norm(V1, cv::NORM_L2);
        V2 = point_2 - point_0;
        nv2 = cv::norm(V2, cv::NORM_L2);
        double Scale = nv2 / nv1;
        Scale = pow(Scale, 3);
        zoomzMats.cwt = Scale * p->pMats.cwt;
        cv::Mat topPart = cv::Mat::zeros(3, 4, CV_64F);
        cv::hconcat(zoomzMats.cwR, zoomzMats.cwt, topPart);
        cv::Mat bottomRow = (cv::Mat_<double>(1, 4) << 0, 0, 0, 1);
        cv::vconcat(topPart, bottomRow, zoomzMats.Pext);
        zoomzMats.Perspective = zoomzMats.Pint * zoomzMats.Pext;
        p->image.refresh();
        p->jack.draw(zoomzMats.Perspective, p->image.display_image);
        cv::circle(p->image.display_image, center, 40, cv::Scalar(0, 0, 255),
                   2);
        cv::circle(p->image.display_image, center, nv1, cv::Scalar(0, 0, 255),
                   2);
        p->image.show();

        rc(13, 0);
        puts("zoomzPmats.cwt");
        print_mat(zoomzMats.cwt);
        printf("(Scale)(%10.4f) nv2(%10.4f),nv1(%10.4f)               ", Scale,
               nv2, nv1);
        fflush(stdout);
      }
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        ZOOMZ_STATE = zoomz_state_e::SAVE_ZOOMZ;
        p->keyLast = -1;
        once = 0;
      }
    }
    break;
    case zoomz_state_e::SAVE_ZOOMZ:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ZOOMZ STATE SAVE_ZOOMZ      's' to save, 'esc' to leave    ");
      }
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        if ('s' == p->keyLast)
        {
          perspec_copy_A2B(zoomzMats, p->pMats);
          ZOOMZ_STATE = zoomz_state_e::DONE;
          p->keyLast = -1;
          once = 0;
        }
        else if (27 == p->keyLast || ' ' == p->keyLast)
        {
          ZOOMZ_STATE = zoomz_state_e::DONE;
          p->keyLast = -1;
          once = 0;
        }
      }
    }
    break;
    case zoomz_state_e::DONE:
    {
      rc(4, 0);
      puts("Zoomz done");
      refresh_lines();
      p->image.refresh();
      p->jack.draw(p->pMats.Perspective, p->image.display_image);
      p->image.show();
      ZOOMZ_STATE = zoomz_state_e::SETUP;
      once = 0;
      return 1;
    }
    break;
    default:
      break;
  }
  return 0;
}
cv::Point3d 
rot2_s::get_arcball_vector(int x, int y, int w, int h)
{
  cv::Point3d p;
  p.x = 1.0 -(2.0 * x / w);
  p.y = 1.0 -(2.0 * y / h);
  p.z = 0.0;

  double mag_sq = p.x * p.x + p.y * p.y;
  if( mag_sq <= 1.0) {
    p.z = std::sqrt(1.0 - mag_sq);
  } else {
    // Normalize if outside circle (edge behavior)
    double scale = 1.0 / std::sqrt(mag_sq);
    p.x *= scale;
    p.y *= scale;
  }
  return p;
}
int
rot2_s::rot2(struct system_s* p, int keyStroke)
{
  if(0 == init)
  {
    init = 1;
    perspec_copy_A2B(p->pMats, rot2Mats);
    rc(5,0);
    printf("rot2");
    fflush(stdout);
  }

  if(p->from_mouse_data.event == cv::EVENT_MBUTTONDOWN)
  {
    rc(6,0);
    printf("mbutton 1");fflush(stdout);
    start_vec = get_arcball_vector(p->from_mouse_data.x, 
      p->from_mouse_data.y,rot2Mats.width,rot2Mats.height);
    is_dragging = 1;
  }
  else if(p->from_mouse_data.event == cv::EVENT_MOUSEMOVE && is_dragging)
  {
    cv::Point3d end_vec = get_arcball_vector(p->from_mouse_data.x,
      p->from_mouse_data.y,rot2Mats.width,rot2Mats.height);

    cv::Point3d axis = start_vec.cross(end_vec);
    double dot = start_vec.dot(end_vec);
    double angle = std::acos(std::max(-1.0, std::min(1.0, dot))); 
    double axis_norm = norm(axis);
    if(axis_norm > 1e-6) {
      cv::Vec3d r_vec = (axis / axis_norm) * angle;

      cv::Mat delta_R;
      cv::Rodrigues(r_vec, delta_R);

      rot2Mats.cwR = delta_R * rot2Mats.cwR;
      cv::Mat topPart = cv::Mat::zeros(3, 4, CV_64F);
      cv::hconcat(rot2Mats.cwR, rot2Mats.cwt, topPart);
      cv::Mat bottomRow = (cv::Mat_<double>(1, 4) << 0, 0, 0, 1);
      cv::vconcat(topPart, bottomRow, rot2Mats.Pext);
      rot2Mats.Perspective = rot2Mats.Pint * rot2Mats.Pext;
      p->image.refresh();
      p->jack.draw(rot2Mats.Perspective, p->image.display_image);
      p->image.show();

      
      start_vec = end_vec;
    }
  }
  else if( p->from_mouse_data.event == cv::EVENT_MBUTTONUP) {
    is_dragging = 0;
  }    

  if('s' == keyStroke)
  {
      perspec_copy_A2B(rot2Mats, p->pMats);
      init = 0;
      return 1;
  }
  if( 27 == keyStroke || ' ' == keyStroke)
  {
      init = 0;
      return 1;
  }




  //if(1 == p->from_mouse_data.new_data) is always true
  /*
  switch (ROT2_STATE)
  {
    case rot2_state_e::SETUP:
    {
      rc(4, 0);
      refresh_lines();
      rc(4, 0);
      printf("ROTATE STATE 'r' to rot2 again, 's' to save rot2, 'esc' to "
             "leave.          ");
      fflush(stdout);

      perspec_copy_A2B(p->pMats, rot2Mats);
      ROT2_STATE = rot2_state_e::P0;
    }
    break;
    case rot2_state_e::P0:
    {
      if (  cv::EVENT_FLAG_MBUTTON & p->from_mouse_data.flag )
      {
        // Middle mouse button down, Store rot2 start point in point_0
        double Z = rot2Mats.cwt.at<double>(0,2);
        center.x = p->from_mouse_data.x;
        center.x = p->from_mouse_data.x;
        point_0.at<double>(0, 0) = p->from_mouse_data.x;
        point_0.at<double>(1, 0) = p->from_mouse_data.y;
        point_0.at<double>(2, 0) = Z;
        cv::circle(p->image.display_image, center, 40, cv::Scalar(0, 0, 255),
                   2);
        p->image.show();
        rc(5, 0);
        puts("P0 ");
        print_mat(point_0);
        static int one_time=0;
        if(one_time==0)
        {
          one_time=1;
        point_0.at<double>(0,0)=(point_0.at<double>(0,0) - rot2Mats.u0)/rot2Mats.fx;
        point_0.at<double>(1,0)=(point_0.at<double>(1,0) - rot2Mats.v0)/rot2Mats.fy;
        }
        puts("P0 scaled (u-uo)/alpha, (v-vo)/beta");
        print_mat(point_0);
        puts("cwt");
        print_mat(p->pMats.cwt);

        if(p->keyLast == 27)
        {
          one_time=0;
          ROT2_STATE= rot2_state_e::DONE;
          puts("ESC received");
        }
        else
        { 
          ROT2_STATE = rot2_state_e::MOUSE_UP; 
          once = 0;
        }
      }
    }
    break;
    case rot2_state_e::MOUSE_UP:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ROTATE2 STATE MOUSE_UP 'r' to rot2 again, 's' to save rot2, "
               "'esc' to leave.          ");
        fflush(stdout);
      }
      // Wait for center mouse button up
      if (!(cv::EVENT_FLAG_MBUTTON & p->from_mouse_data.flag))
      {
        ROT2_STATE = rot2_state_e::P0;
//        ROT2_STATE = rot2_state_e::P1;
        once = 0;
      }
    }
    break;
    case rot2_state_e::P1:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ROTATE2 STATE P1 'r' to rot2 again, 's' to save rot2, 'esc' to "
               "leave.          ");
        fflush(stdout);
      }
      // center mouse button down, store point_1
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        // Store rot2 start point in point_0
        point_1.at<double>(0, 0) =
          p->from_mouse_data.x; // center.x = p->from_mouse_data.x;
        point_1.at<double>(1, 0) =
          p->from_mouse_data.y; // center.y = p->from_mouse_data.y;
        point_1.at<double>(2, 0) = 0;
        rad = cv::norm(point_0, point_1, cv::NORM_L2);
        cv::circle(p->image.display_image, center, (int)rad,
                   cv::Scalar(0, 0, 255), 2);
        p->image.show();
        rc(9, 0);
        puts("P1 bigger circle?");
        print_mat(point_0);
        // ROT2_STATE = rot2_state_e::MOUSE_UP2;
        ROT2_STATE = rot2_state_e::ROT2;
        once = 0;
      }
    }
    break;
    case rot2_state_e::MOUSE_UP2:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ROTATE STATE M_UP2 'r' to rot2 again, 's' to save rot2, 'esc' "
               "to leave.          ");
        fflush(stdout);
      }
      // wait middle mouse button up
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        ROT2_STATE = rot2_state_e::ROT2;
        once = 0;
      }
    }
    break;
    case rot2_state_e::ROT2:
    {
      // middle mouse button down:
      // calculate rotation vector =
      // v = cross( normalized(p2-p0),zVector)*(1-|p2-p0]/|p1-p0|)
      // cwR = Rotv(v) * cwR
      // P = Pint * [cwR,cwt;[0 0 0 1]]
      // refresh
      // draw circle 0, draw circle 1 on display_image
      // jackDraw(P,display_image
      // show, next round until mouse up
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ROTATE STATE ROT2 'r' to rot2 again, 's' to save rot2, 'esc' "
               "to leave.          ");
        fflush(stdout);
      }
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        point_2.at<double>(0, 0) =
          p->from_mouse_data.x; // center.x = p->from_mouse_data.x;
        point_2.at<double>(1, 0) =
          p->from_mouse_data.y; // center.y = p->from_mouse_data.y;
        point_2.at<double>(2, 0) = 0;
        V1 = point_1 - point_0;
        nv1 = cv::norm(V1, cv::NORM_L2);
        V2 = point_2 - point_0;
        nv2 = cv::norm(V2, cv::NORM_L2);
        cv::Mat Vz = (cv::Mat_<double>(3, 1) << 0, 0, 1);
        // cross product: z x Vr
        Vr = Vz.cross(V2 / nv2);
        nvr = cv::norm(Vr, cv::NORM_L2);
        Vr = Vr / nvr;
        double Scale = .2 * (1.0 - nv2 / rad);
        // Scale = pow(Scale,3);
        Vr = M_PI * (Scale * Vr + .2 * V1.cross(V2 / nv2) / nv1);
        Rodrigues(Vr, R);
        cv::Mat Rt = p->pMats.cwR.clone();
        Rt = Rt.t();
        // rot2Mats.cwR = R * p->pMats.cwR;
        Rt = R * Rt; // p->pMats.cwR;
        rot2Mats.cwR = Rt.t();
        cv::Mat topPart = cv::Mat::zeros(3, 4, CV_64F);
        cv::hconcat(rot2Mats.cwR, rot2Mats.cwt, topPart);
        cv::Mat bottomRow = (cv::Mat_<double>(1, 4) << 0, 0, 0, 1);
        cv::vconcat(topPart, bottomRow, rot2Mats.Pext);
        rot2Mats.Perspective = rot2Mats.Pint * rot2Mats.Pext;
        p->image.refresh();
        p->jack.draw(rot2Mats.Perspective, p->image.display_image);
        cv::circle(p->image.display_image, center, 40, cv::Scalar(0, 0, 255),
                   2);
        cv::circle(p->image.display_image, center, rad, cv::Scalar(0, 0, 255),
                   2);
        p->image.show();

        rc(13, 0);
        puts("Vr");
        print_mat(Vr);
        printf("(1-nvr/rad)(%10.4f) nvr(%10.4f),rad(%10.4f)               ",
               Scale, nvr, rad);
        fflush(stdout);
      }
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        ROT2_STATE = rot2_state_e::SAVE;
        once = 0;
      }
    }
    break;
    case rot2_state_e::SAVE:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        p->keyLast = -1;
        printf("ROTATE STATE SAVE 'r' to rot2 again, 's' to save rot2, 'esc' "
               "to leave.          ");
        fflush(stdout);
      }
      if (p->keyLast == 's')
      {
        perspec_copy_A2B(rot2Mats, p->pMats);
        ROT2_STATE = rot2_state_e::DONE;
      }
      else if (p->keyLast == 27)
      {
        ROT2_STATE = rot2_state_e::DONE;
      }
    }
    break;
    case rot2_state_e::DONE:
    {
      rc(4, 0);
      refresh_lines();
      ROT2_STATE = rot2_state_e::SETUP;
      p->image.refresh();
      p->jack.draw(p->pMats.Perspective, p->image.display_image);
      p->image.show();
      once = 1;
      return 1;
    }
    break;
    default:
      break;
  }
      */
  return 0;
}
int
rot_s::rot(struct system_s* p, int keyStroke)
{
  switch (ROT_STATE)
  {
    case rot_state_e::SETUP:
    {
      rc(4, 0);
      printf("ROTATE STATE 'r' to rot again, 's' to save rot, 'esc' to leave.  "
             "        ");
      fflush(stdout);

      perspec_copy_A2B(p->pMats, rotMats);
      ROT_STATE = rot_state_e::P0;
    }
    break;
    case rot_state_e::P0:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ROTATE STATE P0 'r' to rot again, 's' to save rot, 'esc' to "
               "leave.          ");
        fflush(stdout);
      }
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        // Middle mouse button down, Store rot start point in point_0
        point_0.at<double>(0, 0) = center.x = p->from_mouse_data.x;
        point_0.at<double>(1, 0) = center.y = p->from_mouse_data.y;
        point_0.at<double>(2, 0) = 0;
        cv::circle(p->image.display_image, center, 40, cv::Scalar(0, 0, 255),
                   2);
        p->image.show();
        rc(5, 0);
        puts("P0 ");
        print_mat(point_0);
        ROT_STATE = rot_state_e::MOUSE_UP;
        once = 0;
      }
    }
    break;
    case rot_state_e::MOUSE_UP:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ROTATE STATE MOUSE_UP 'r' to rot again, 's' to save rot, 'esc' "
               "to leave.          ");
        fflush(stdout);
      }
      // Wait for center mouse button up
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        ROT_STATE = rot_state_e::P1;
        once = 0;
      }
    }
    break;
    case rot_state_e::P1:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ROTATE STATE P1 'r' to rot again, 's' to save rot, 'esc' to "
               "leave.          ");
        fflush(stdout);
      }
      // center mouse button down, store point_1
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        // Store rot start point in point_0
        point_1.at<double>(0, 0) =
          p->from_mouse_data.x; // center.x = p->from_mouse_data.x;
        point_1.at<double>(1, 0) =
          p->from_mouse_data.y; // center.y = p->from_mouse_data.y;
        point_1.at<double>(2, 0) = 0;
        rad = cv::norm(point_0, point_1, cv::NORM_L2);
        cv::circle(p->image.display_image, center, (int)rad,
                   cv::Scalar(0, 0, 255), 2);
        p->image.show();
        rc(9, 0);
        puts("P1 bigger circle?");
        print_mat(point_0);
        // ROT_STATE = rot_state_e::MOUSE_UP2;
        ROT_STATE = rot_state_e::ROT;
        once = 0;
      }
    }
    break;
    case rot_state_e::MOUSE_UP2:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ROTATE STATE M_UP2 'r' to rot again, 's' to save rot, 'esc' to "
               "leave.          ");
        fflush(stdout);
      }
      // wait middle mouse button up
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        ROT_STATE = rot_state_e::ROT;
        once = 0;
      }
    }
    break;
    case rot_state_e::ROT:
    {
      // middle mouse button down:
      // calculate rotation vector =
      // v = cross( normalized(p2-p0),zVector)*(1-|p2-p0]/|p1-p0|)
      // cwR = Rotv(v) * cwR
      // P = Pint * [cwR,cwt;[0 0 0 1]]
      // refresh
      // draw circle 0, draw circle 1 on display_image
      // jackDraw(P,display_image
      // show, next round until mouse up
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        printf("ROTATE STATE ROT 'r' to rot again, 's' to save rot, 'esc' to "
               "leave.          ");
        fflush(stdout);
      }
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        point_2.at<double>(0, 0) =
          p->from_mouse_data.x; // center.x = p->from_mouse_data.x;
        point_2.at<double>(1, 0) =
          p->from_mouse_data.y; // center.y = p->from_mouse_data.y;
        point_2.at<double>(2, 0) = 0;
        V1 = point_1 - point_0;
        nv1 = cv::norm(V1, cv::NORM_L2);
        V2 = point_2 - point_0;
        nv2 = cv::norm(V2, cv::NORM_L2);
        cv::Mat Vz = (cv::Mat_<double>(3, 1) << 0, 0, 1);
        // cross product: z x Vr
        Vr = Vz.cross(V2 / nv2);
        nvr = cv::norm(Vr, cv::NORM_L2);
        Vr = Vr / nvr;
        double Scale = .2 * (1.0 - nv2 / rad);
        // Scale = pow(Scale,3);
        Vr = M_PI * (Scale * Vr + .2 * V1.cross(V2 / nv2) / nv1);
        Rodrigues(Vr, R);
        cv::Mat Rt = p->pMats.cwR.clone();
        Rt = Rt.t();
        // rotMats.cwR = R * p->pMats.cwR;
        Rt = R * Rt; // p->pMats.cwR;
        rotMats.cwR = Rt.t();
        cv::Mat topPart = cv::Mat::zeros(3, 4, CV_64F);
        cv::hconcat(rotMats.cwR, rotMats.cwt, topPart);
        cv::Mat bottomRow = (cv::Mat_<double>(1, 4) << 0, 0, 0, 1);
        cv::vconcat(topPart, bottomRow, rotMats.Pext);
        rotMats.Perspective = rotMats.Pint * rotMats.Pext;
        p->image.refresh();
        p->jack.draw(rotMats.Perspective, p->image.display_image);
        cv::circle(p->image.display_image, center, 40, cv::Scalar(0, 0, 255),
                   2);
        cv::circle(p->image.display_image, center, rad, cv::Scalar(0, 0, 255),
                   2);
        p->image.show();

        rc(13, 0);
        puts("Vr");
        print_mat(Vr);
        printf("(1-nvr/rad)(%10.4f) nvr(%10.4f),rad(%10.4f)               ",
               Scale, nvr, rad);
        fflush(stdout);
      }
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        ROT_STATE = rot_state_e::SAVE;
        once = 0;
      }
    }
    break;
    case rot_state_e::SAVE:
    {
      if (0 == once)
      {
        once = 1;
        rc(4, 0);
        p->keyLast = -1;
        printf("ROTATE STATE SAVE 'r' to rot again, 's' to save rot, 'esc' to "
               "leave.          ");
        fflush(stdout);
      }
      if (p->keyLast == 's')
      {
        perspec_copy_A2B(rotMats, p->pMats);
        ROT_STATE = rot_state_e::DONE;
      }
      else if (p->keyLast == 27 || p->keyLast == ' ')
      {
        ROT_STATE = rot_state_e::DONE;
      }
    }
    break;
    case rot_state_e::DONE:
    {
      rc(4, 0);
      refresh_lines();
      ROT_STATE = rot_state_e::SETUP;
      p->image.refresh();
      p->jack.draw(p->pMats.Perspective, p->image.display_image);
      p->image.show();
      once = 1;
      return 1;
    }
    break;
    default:
      break;
  }
  return 0;
}
int
pan_s::pan(struct system_s* p, int keyStroke)
{

  static int loop_cnt = 0;
  ++loop_cnt;
  switch (PAN_STATE)
  {
    case pan_state_e::SETUP:
    {
      // puts("PAN STATE");
      perspec_copy_A2B(p->pMats, panMats);
      PAN_STATE = pan_state_e::P0;
      rc(4, 0);
      printf("PAN STATE 's' to save pan, 'esc' to leave no save.          ");
      fflush(stdout);
    }
    break;
    case pan_state_e::P0:
    {
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        // Store pan start point in point_0
        p->pan.point_0.at<double>(0, 0) = p->from_mouse_data.x;
        p->pan.point_0.at<double>(1, 0) = p->from_mouse_data.y;
        p->pan.point_0.at<double>(2, 0) = 0;
        rc(5, 0);
        puts("P0 ");
        print_mat(p->pan.point_0);
        PAN_STATE = pan_state_e::P1;
      }
      // else stay in P0
    }
    break;
    case pan_state_e::P1:
    {
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        // Store current point to point_1 and draw panned "jack"
        point_1.at<double>(0, 0) = p->from_mouse_data.x;
        point_1.at<double>(1, 0) = p->from_mouse_data.y;
        point_1.at<double>(2, 0) = 0;
        dP = point_1 - point_0;
        // get distance z, note: Pworld = z*[u,v,1]' / focal_length
        double z = panMats.cwt.at<double>(2, 0); // octave row 3, col 1
        // generate displacement of {W} along cwt
        dP = z * dP / panMats.fx;
        // update pan's copy of cwt from systems {W} pMats
        panMats.cwt = p->pMats.cwt + dP;
        // Build perspective matrix
        cv::Mat topPart = cv::Mat::zeros(3, 4, CV_64F);
        cv::hconcat(panMats.cwR, panMats.cwt, topPart);
        cv::Mat bottomRow = (cv::Mat_<double>(1, 4) << 0, 0, 0, 1);
        cv::vconcat(topPart, bottomRow, panMats.Pext);
        panMats.Perspective = panMats.Pint * panMats.Pext;
        //rc(9, 0);
        rc(5, 12);
        puts("P1: updating.");
        print_matRC(p->pan.point_1,6,12);
        // redraw jack wireframe onto new copy of display_image
        p->image.refresh();
        p->jack.draw(panMats.Perspective, p->image.display_image);
        p->image.show();
      }
      else if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        PAN_STATE = pan_state_e::CALC;
      }
    }
    break;
    case pan_state_e::CALC:
    {
      if (-1 != keyStroke)
      {
        // printf("keyStroke(%d,%c)\n",keyStroke,(char)keyStroke);
        switch (keyStroke)
        {
          // case 'p': PAN_STATE = pan_state_e::P0;break;
          case 's':
            PAN_STATE = pan_state_e::SAVE_PAN;
            break;
          case 27:
          case ' ':
            PAN_STATE = pan_state_e::DONE;
            break;
          default:
            break;
        }
      }
    }
    break;
    case pan_state_e::SAVE_PAN:
    {
      perspec_copy_A2B(panMats, p->pMats);
      PAN_STATE = pan_state_e::DONE;
    }
    break;
    case pan_state_e::DONE:
    {
      // Leave state machine at starting point, return with result
      PAN_STATE = pan_state_e::SETUP;
      rc(4, 0);
      refresh_lines();
      p->image.refresh();
      p->jack.draw(p->pMats.Perspective, p->image.display_image);
      p->image.show();

      return 1;
    }
    break;
    default:
      break;
  }

  return 0;
}
void
system_s::print_mouse_data(int loop_cnt, int keyStroke, int keyLast)
{
  // Display the inputs on Highgui's terminal
  if ((from_mouse_data.new_data == 1) || (keyStroke != -1))
  {
    rc(1, 0);
    printf("Mousedata: x(%4d) y(%4d) event(%d) flag(%d) new_data(%d)             "
           "keyLast(%d,%c), STATE(%d) loop_cnt(%d) keyStroke(%d)\n",
           from_mouse_data.x, from_mouse_data.y, from_mouse_data.event,
           from_mouse_data.flag, from_mouse_data.new_data, keyLast,
           (char)keyLast, (int)STATE, loop_cnt, keyStroke);
    printf("last data: x(%4d) y(%4d) event(%d) flag(%d) new_data(%d)             "
           "keyLast(%d,%c), STATE(%d) loop_cnt(%d) keyStroke(%d)\n",
           last_mouse_data.x, last_mouse_data.y, last_mouse_data.event,
           last_mouse_data.flag, last_mouse_data.new_data, keyLast,
           (char)keyLast, (int)STATE, loop_cnt, keyStroke);
    // from_mouse_data.x-(int)pMats.u0, from_mouse_data.y-(int)pMats.v0,
    // from_mouse_data.event,
  }
}
void
system_s::get_mouse_data(void)
{
  last_mouse_data = from_mouse_data;
  from_mouse_data.new_data=0;// = zero_mouse_data;
  { // Mutex lock active in this scope limiting set of braces
    std::lock_guard<std::mutex> lock(mouse_mtx);
    if (1 == mouse_data.new_data)
    {
      from_mouse_data = mouse_data;
      mouse_data.new_data = 0;
    }
  } // Mutex lock ends at end of scope parenthesis
}
void
system_s::init(void)
{

  zero_mouse_data.event = 0;
  zero_mouse_data.x = 0;
  zero_mouse_data.y = 0;
  zero_mouse_data.new_data = 0;
  last_mouse_data=zero_mouse_data;
  from_mouse_data=zero_mouse_data;
  mouse_data=zero_mouse_data;
}
void
perspec_s::init(void)
{
  // clang-format off
  K = (cv::Mat_<double>(3,3)<< fx,0,u0, 0, fy, v0, 0,0,1);
  Pint = (cv::Mat_<double>(3,4)<<  fx, 0, u0, 0, 
                                    0,fy, v0, 0, 
                                    0,  0, 1, 0);
  // clang-format on
  cwk = (cv::Mat_<double>(3, 1) << 0, 0, 0);
  cwt = (cv::Mat_<double>(3, 1) << 0, 0, 0.2);
  cwR = cv::Mat::zeros(3, 3, CV_64F);
  Pext = cv::Mat::zeros(4, 4, CV_64F);
  Perspective = cv::Mat::zeros(3, 4, CV_64F);
  cv::Mat topPart;
  const cv::Mat bottomRow = (cv::Mat_<double>(1, 4) << 0, 0, 0, 1);
  cv::Rodrigues(cwk, cwR);
  cv::hconcat(cwR, cwt, topPart);
  cv::vconcat(topPart, bottomRow, Pext);
  Perspective = Pint * Pext;
}
void
perspec_copy_A2B(const struct perspec_s A, struct perspec_s& B)
{
  B.width = A.width;
  B.height = A.height;
  B.fx = A.fx;
  B.fy = A.fy;
  B.u0 = A.u0;
  A.cwR.copyTo(B.cwR);
  A.cwk.copyTo(B.cwk);
  A.cwt.copyTo(B.cwt);
  A.Pint.copyTo(B.Pint);
  A.Pext.copyTo(B.Pext);
  A.K.copyTo(B.K);
  A.Perspective.copyTo(B.Perspective);
}

void
print_mat(const cv::Mat p)
{
  // std::cout << "p.type() = " << p.type() << "\n";
  my_assert(p.type() == CV_64F, __LINE__, __FILE__);
  for (int i = 0; i < p.rows; ++i)
  {
    for (int j = 0; j < p.cols; ++j)
    {
      printf("%10.3e, ", p.at<double>(i, j));
    }
    puts("");
  }
}
void
print_matRC(const cv::Mat p,int row, int col)
{
  // std::cout << "p.type() = " << p.type() << "\n";
  my_assert(p.type() == CV_64F, __LINE__, __FILE__);
  for (int i = 0; i < p.rows; ++i)
  {
    rc(row+i,col);
    for (int j = 0; j < p.cols; ++j)
    {
      printf("%10.3e, ", p.at<double>(i, j));
    }
    puts("");
  }
}
void
fprint_mat(const cv::Mat p,const char *p_str)
{
  // std::cout << "p.type() = " << p.type() << "\n";
  my_assert(p.type() == CV_64F, __LINE__, __FILE__);
  static int once = 0;
  FILE *outFile;
  if(0 == once)
  { 
    once = 1;
    outFile = fopen("Camera_outFile.m","w");
  } else {
    outFile = fopen("Camera_outFile.m","a");
  }
  my_assert(NULL != outFile,__LINE__,__FILE__);
  fprintf(outFile,"%s=[\n",p_str);
  for (int i = 0; i < p.rows; ++i)
  {
    for (int j = 0; j < p.cols; ++j)
    {
      fprintf(outFile,"%.17g, ", p.at<double>(i, j));
    }
    fprintf(outFile,"\n");
  }
  fprintf(outFile,"];\n");
  fclose(outFile);
}

void
image_data_s::load(void)
{
  w_name = std::string("Image 0");
  cv::namedWindow(w_name, cv::WINDOW_NORMAL);
  cv::resizeWindow(w_name, 800, 1200);
  imageFile = std::string("IMG_20251119_105708393.jpg");
  original_image = cv::imread(imageFile);
  display_image = original_image.clone();
  // cv::imshow(w_name,display_image);
};

void
image_data_s::show(void)
{
  cv::imshow(w_name, display_image);
}
void
image_data_s::refresh(void)
{
  original_image.copyTo(display_image);
  cv::imshow(w_name, display_image);
}

void
perspec_s::print(void)
{
  puts("Perspecive");
  print_mat(Perspective);
  puts("Pint");
  print_mat(Pint);
  puts("Pext");
  print_mat(Pext);
  puts("cwk ");
  print_mat(cwk);
  puts("cwt");
  print_mat(cwt);
}
int
points_s::read_jack_pts(struct system_s* p, int keyStroke)
{
  //test
  print_DLT_equations(p);
  return 1;
  switch (POINTS_STATE)
  {
    case points_state_e::SETUP:
    {
      indx_i = indx_j = 0;
      for (int i = 0; i < 17; i++)
      {
        for (int j = 0; j < 3; j++)
        {
          obj_points.at<double>(i, j) = p->jack.vertices.at<double>(j, i);
        }
      }
      img_points = cv::Mat::zeros(17, 3, CV_64F);
      rc(2, 0);
      puts("obj_points");
      print_mat(obj_points);
      if (0)
      {
        puts("vertices");
        print_mat(p->jack.vertices.t());
        puts("obj_points");
        print_mat(obj_points);
        return 1;
      }
      POINTS_STATE = points_state_e::READ_THIS;
    }
    break;
    case points_state_e::READ_THIS:
    {
      rc(20, 0);
      printf("index %d point (%f, %f, %f)                           ", indx_i,
             100 * obj_points.at<double>(indx_i, 0),
             100 * obj_points.at<double>(indx_i, 1),
             100 * obj_points.at<double>(indx_i, 2));
      fflush(stdout);
      POINTS_STATE = points_state_e::READ_PT;
    }
    break;
    case points_state_e::READ_PT:
    {
      if (1 == p->from_mouse_data.new_data && 4 == p->from_mouse_data.flag)
      {
        img_points.at<double>(indx_i, 0) = p->from_mouse_data.x;
        img_points.at<double>(indx_i, 1) = p->from_mouse_data.y;
        rc(21, 0);
        printf("indx = %d", indx_i);
        fflush(stdout);
        ++indx_i;
        if (indx_i < 17)
        {
          POINTS_STATE = points_state_e::MOUSE_UP;
        }
        else
        {
          POINTS_STATE = points_state_e::DONE;
          // cv::FileStorage fs2("pMats.xml", cv::FileStorage::READ);
          //  //fs2.release();
        }
      }
    }
    break;
    case points_state_e::MOUSE_UP:
    {
      if (1 == p->from_mouse_data.new_data && 4 != p->from_mouse_data.flag)
      {
        POINTS_STATE = points_state_e::READ_THIS;
      }
    }
    break;
    case points_state_e::DONE:
    {
      rc(5, 0);
      refresh_lines();
      rc(20, 0);
      printf("Done collecting points. Saving as cal_points.xml\n");
      cv::FileStorage fs("cal_points.xml", cv::FileStorage::WRITE);
      fs << "img_points" << img_points;
      fs << "obj_points" << obj_points;
      fs.release();
      FILE* pOutFile = fopen("cal_data.txt", "w");
      fprintf(pOutFile, "%% cal data [obj_points(17,3), img_points(17,2)\n"
                        "d = [\n");
      for (int i = 0; i < 17; i++)
      {
        fprintf(pOutFile, "%10.4f %10.4f %10.4f %10.4f %10.4f\n",
                obj_points.at<double>(i, 0), obj_points.at<double>(i, 1),
                obj_points.at<double>(i, 2), img_points.at<double>(i, 0),
                img_points.at<double>(i, 1));
      }
      fprintf(pOutFile, "];\n");

      fprintf(pOutFile,"%% \nA=[\n");

      for(int i = 0; i<17; i++)
      {
        fprintf(pOutFile,
          "%12.8f %12.8f  %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f\n "
          "%12.8f %12.8f  %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f\n ",
          obj_points.at<double>(i,0), 
          obj_points.at<double>(i,0), 
          obj_points.at<double>(i,0),
          1.0,
          0.0,0.0,0.0,0.0,
          -((double)img_points.at<double>(i,0))*obj_points.at<double>(i,0),
          -((double)img_points.at<double>(i,0))*obj_points.at<double>(i,1),
          -((double)img_points.at<double>(i,0))*obj_points.at<double>(i,2),
          -((double)img_points.at<double>(i,0)),
          0.0,0.0,0.0,0.0,
          obj_points.at<double>(i,0), 
          obj_points.at<double>(i,0), 
          obj_points.at<double>(i,0),
          1.0,
          -((double)img_points.at<double>(i,1))*obj_points.at<double>(i,0),
          -((double)img_points.at<double>(i,1))*obj_points.at<double>(i,1),
          -((double)img_points.at<double>(i,1))*obj_points.at<double>(i,2),
          -((double)img_points.at<double>(i,1)));
      }
      fprintf(pOutFile, "];\n");


      fclose(pOutFile);
      POINTS_STATE = points_state_e::SETUP;
      return 1;
    }
    break;
    default:
      break;
  }
  return 0;
}
void print_DLT_equations(system_s *p)
{
/*          (cv::Mat_<double>(3, 4) << zoomfMats.fx, 0, zoomfMats.u0, 0, 0, zoomfMats.fy, zoomfMats.v0, 0, 0, 0, 1, 0);
      cv::Size isize = p->image.original_image.size();
      cv::Mat M,R,Q,Qx,Qy,Qz;
      cv::RQDecomp3x3(M,R,Q,Qx,Qy,Qz);
      */
      fprint_mat(p->pMats.cwR,"cwR");
      fprint_mat(p->pMats.cwt,"cwt");
      fprint_mat(p->pMats.K,"K");
      fprint_mat(p->pMats.Pint,"Pint");
      fprint_mat(p->pMats.Pext,"Pext");

      fprint_mat(p->pMats.Perspective,"Perspective");
      cv::Mat DLT= (cv::Mat_<double>(34,12));
      DLT = cv::Scalar(0);  // both vertices and image_vertices are 3 or 4 x17
      const cv::Mat uv = p->jack.image_vertices; // N x 3 homogeneous image vectors normalized
      const cv::Mat P  = p->jack.vertices;       // N x 4 homogeneous object vectors normalize
      for(int i=0; i<17 ; i++)
      {
        // build rows of DLT that alternate with  P^T(1,4)   zeros(1,4)  -u *P^T(1,4)
        //                                    and zeros1(,4) P^T(1,4)    -v *P^T(1,4)
        // DLT is 2*17 x 12      P is 4 x 17, and uv is 3 x 17
        // each for loop is 1x4 slot of DLT row getting again P^T zeros -u*P^T ... (as above)
        for(int j=0; j<4; j++) DLT.at<double>(2*i,j)    = P.at<double>(j,i);
        for(int j=0; j<4; j++) DLT.at<double>(2*i,j+4)  = 0.0;
        for(int j=0; j<4; j++) DLT.at<double>(2*i,j+8)  = -uv.at<double>(0,i) * P.at<double>(j,i);

        for(int j=0; j<4; j++) DLT.at<double>(2*i+1,j)  = 0.0;
        for(int j=0; j<4; j++) DLT.at<double>(2*i+1,j+4)= P.at<double>(j,i);
        for(int j=0; j<4; j++) DLT.at<double>(2*i+1,j+8)= -uv.at<double>(1,i) * P.at<double>(j,i);
      }


      /*
      for(int i=0; i<17; i++)
      {
        for(int j=0; j<4; j++)  DLT.at<double>(2*i,j)     = p->jack.vertices.at<double>(j,i);
        for(int j=0; j<4; j++)  DLT.at<double>(2*i,j+4)   = 0.0; // already 0
        for(int j=0; j<4; j++)  DLT.at<double>(2*i,j+8)   = -p->jack.image_vertices.at<double>(0,i)
                                                            *p->jack.vertices.at<double>(j,i);
        
        for(int j=0; j<4; j++)  DLT.at<double>(2*i+1,j)   = 0.0; // already 0
        for(int j=0; j<4; j++)  DLT.at<double>(2*i+1,j+4) = p->jack.vertices.at<double>(j,i);
        for(int j=0; j<4; j++)  DLT.at<double>(2*i+1,j+8) = -p->jack.image_vertices.at<double>(1,i)
                                                            *p->jack.vertices.at<double>(j,i);
      }
      */
      rc(4,0);
      refresh_lines();
      rc(4,0);
      //puts("DLT = [");
      fprint_mat(p->jack.vertices,"vertices");
      fprint_mat(p->jack.image_vertices,"image_vertices");
      fprint_mat(DLT,"DLT");


      return;


      puts("];");
      printf("vertcices rows(%d) cols(%d) \n image_vertices row(%d) cols(%d)\n",
        p->jack.vertices.rows,p->jack.vertices.cols,
        p->jack.image_vertices.rows,p->jack.image_vertices.cols);
        return;  

      rc(7,0);
      printf("vertcices rows(%d) cols(%d) \n image_vertices row(%d) cols(%d)\n",
        p->jack.vertices.rows,p->jack.vertices.cols,
        p->jack.image_vertices.rows,p->jack.image_vertices.cols);


      printf("image_vertices = [\n");
      for(int i = 0;i < 3; i++)
      {
        for(int j=0; j<17; j++)
        {
          printf("%12.6f ",
            p->jack.image_vertices.at<double>(i,j));
        }
        printf("\n");
      }
      printf("];\n");

      printf("vertices = [\n");
      for(int i=0;i<4;i++)
      {
        for(int j=0;j<17;j++)
        {
          printf("%12.6f ", p->jack.vertices.at<double>(i,j)); 
        }
        printf("\n");
      }
      printf("];\n");
          
//        return;

      puts("vertices"); 
      print_mat(p->jack.vertices); 
      puts("image_vertices"); 
      print_mat(p->jack.image_vertices);

      FILE* pOutFile = fopen("DLT.m", "w");
      my_assert(NULL != pOutFile,__LINE__,__FILE__);
      fprintf(pOutFile, "%% DLT A-matrix\n");
      fprintf(pOutFile, "A=[\n"); 
      for(int i = 0; i<17; i++)
      {
        fprintf(pOutFile,
          "%12.8f %12.8f  %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f\n "
          "%12.8f %12.8f  %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f\n ",
          p->jack.vertices.at<double>(0,i), 
          p->jack.vertices.at<double>(1,i), 
          p->jack.vertices.at<double>(2,i), 
          p->jack.vertices.at<double>(3,i), 
          0.0,0.0,0.0,0.0,
          -p->jack.image_vertices.at<double>(0,i)*p->jack.vertices.at<double>(0,i),
          -p->jack.image_vertices.at<double>(0,i)*p->jack.vertices.at<double>(1,i),
          -p->jack.image_vertices.at<double>(0,i)*p->jack.vertices.at<double>(2,i),
          -p->jack.image_vertices.at<double>(0,i)*p->jack.vertices.at<double>(3,i),
          0.0,0.0,0.0,0.0,
          p->jack.vertices.at<double>(0,i), 
          p->jack.vertices.at<double>(1,i), 
          p->jack.vertices.at<double>(2,i), 
          p->jack.vertices.at<double>(3,i), 
          -p->jack.image_vertices.at<double>(1,i)*p->jack.vertices.at<double>(0,i),
          -p->jack.image_vertices.at<double>(1,i)*p->jack.vertices.at<double>(1,i),
          -p->jack.image_vertices.at<double>(1,i)*p->jack.vertices.at<double>(2,i),
          -p->jack.image_vertices.at<double>(1,i)*p->jack.vertices.at<double>(3,i)
        );
      }
      fprintf(pOutFile, "];\n");
      fclose(pOutFile);
}
void
jack_wireframe_s::build()
{
  vertices = (cv::Mat_<double>(17, 3) << // vertices(x,y,z) //  vertice indx
                Lx,
              0, 0,        // 0
              Lx, 0, cs,   // 1
              cs, 0, cs,   // 2
              cs, 0, Hz,   // 3
              0, 0, Hz,    // 4
              0, cs, Hz,   // 5
              0, cs, cs,   // 6
              0, Wy, cs,   // 7
              0, Wy, 0,    // 8
              0, 0, 0,     // 9
              Lx, cs, 0,   // 10
              Lx, cs, cs,  // 11
              cs, cs, cs,  // 12
              cs, Wy, cs,  // 13
              cs, Wy, 0,   // 14
              cs, cs, 0,   // 15
              cs, cs, Hz); // 16
  {
    cv::Mat jw_T;
    cv::Rodrigues(jw_rvec, jw_T);
    cv::Mat ZerosRow = cv::Mat::zeros(1, 3, CV_64F);
    cv::vconcat(jw_T, ZerosRow, jw_T);
    cv::hconcat(jw_T, jw_tvec, jw_T);
    vertices = vertices.t();
    my_assert(3 == vertices.rows && 17 == vertices.cols, __LINE__, __FILE__);
    cv::Mat OnesRow = cv::Mat::ones(1, 17, CV_64F);
    cv::vconcat(vertices, OnesRow, vertices);
    vertices = jw_T * vertices;
    my_assert(4 == vertices.rows && 17 == vertices.cols, __LINE__, __FILE__);
  }
  // Lines, line takes two points to draw between, these are the vertices of the
  // line indexed above
  line_indx = (cv::Mat_<double>(28, 2) << 0, 1, // a
               1, 2,                            // b
               2, 3,                            // c
               3, 4,                            // d
               4, 5,                            // e
               5, 6,                            // f
               6, 7,                            // g
               7, 8,                            // h
               8, 9,                            // i
               9, 0,                            // j
               9, 4,                            // k
               10, 11,                          // l
               11, 12,                          // m
               12, 13,                          // n
               13, 14,                          // o
               14, 15,                          // p
               15, 10,                          // q
               15, 12,                          // r
               0, 10,                           // s
               1, 11,                           // t
               16, 3,                           // u
               16, 5,                           // v
               15, 16,                          // w
               9, 4,                            // x
               14, 8,                           // y
               13, 7,                           // z
               12, 6,                           // A
               12, 2);                          // B
}
void
jack_wireframe_s::print()
{
  puts("jw_rvec");
  print_mat(jw_rvec);
  puts("jw_tvec");
  print_mat(jw_tvec);
  puts("vertices");
  print_mat(vertices.t());
  puts("line_indx");
  print_mat(line_indx);
}

void
jack_wireframe_s::draw(cv::Mat Perspective, cv::Mat& display_image)
{
  {
    // if(1){puts("vertices"); print_mat(vertices);}
    // std::cout << "vertices.size() =" << vertices.size() << "\n
    // Perspective.size() = \n" << Perspective.size() << "\n";

    if (0)
    {
      puts("draw Perspective =");
      print_mat(Perspective);
    }
    image_vertices = Perspective * vertices;
    if (0)
    {
      puts("unnormed  image_vertices");
      print_mat(image_vertices);
    }
    // generate normalized image vertices
    {
      int cols = image_vertices.cols, rows = image_vertices.rows;
      for (int j = 0; j < cols; ++j)
      {
        double z = image_vertices.at<double>(rows - 1, j);
        for (int i = 0; i < rows; ++i)
        {
          image_vertices.at<double>(i, j) /= z;
        }
      }
    }
    if (0)
    {
      puts("image_vertices = ");
      print_mat(image_vertices);
    }
  }

  // put normalized vertices into cv::Point
  {
    image_vertice_points.clear();
    int cols = image_vertices.cols; //, rows = p.jack_image_vertices_norm.rows;
    for (int j = 0; j < cols; ++j)
    {
      int x_int = static_cast<int>(image_vertices.at<double>(0, j));
      int y_int = static_cast<int>(image_vertices.at<double>(1, j));
      image_vertice_points.emplace_back(x_int, y_int);
    }
    if (0)
      std::cout << "image_vertice_points = \n" << image_vertice_points << "\n";
  }
  int line_cnt = line_indx.rows;
  // printf("line_cnt(%d) %d    %d
  // \n",line_cnt,(int)line_indx.at<double>(1,0),(int)line_indx.at<double>(1,1));
  // int x_axis_indx = 16, y_axis_indx = 15, z_axis_indx = 22;
  int x_axis_indx = 16, y_axis_indx = 15, z_axis_indx = 22;
  for (int row_indx_line_vertices = 0; row_indx_line_vertices < line_cnt;
       ++row_indx_line_vertices)
  {

    int pt_indx_A = (int)line_indx.at<double>(row_indx_line_vertices, 0);
    int pt_indx_B = (int)line_indx.at<double>(row_indx_line_vertices, 1);
    // printf(" row_indx_line_vertices(%d) A(%d) B(%d)
    // \n",row_indx_line_vertices,pt_indx_A,pt_indx_B);

    cv::Point pt_A = image_vertice_points[pt_indx_A];
    cv::Point pt_B = image_vertice_points[pt_indx_B];
    // std::cout << "pt indx("<<pt_indx_A << " " << pt_indx_B << "line= " <<
    // row_indx_line_vertices << "  pt_A(" << pt_A <<")  pt_B(" << pt_B <<
    // ")\n";
    if (z_axis_indx == row_indx_line_vertices)
    { // draw blue line
      line(display_image, pt_A, pt_B, cv::Scalar(255, 0, 0), 3);
    }
    else if (x_axis_indx == row_indx_line_vertices)
    { // red line
      line(display_image, pt_A, pt_B, cv::Scalar(0, 0, 255), 3);
    }
    else if (y_axis_indx == row_indx_line_vertices)
    { // green line
      line(display_image, pt_A, pt_B, cv::Scalar(0, 255, 0), 3);
    }
    else
    { // black line
      line(display_image, pt_A, pt_B, cv::Scalar(0, 0 * 255, 0), 3);
    }
  }
}

void
onMouse(int event, int x, int y, int flag, void* userdata)
{
  {
    static system_s* p = (system_s*)userdata;
    std::lock_guard<std::mutex> lock(p->mouse_mtx);
    p->mouse_data.x = x;
    p->mouse_data.y = y;
    p->mouse_data.event = event;
    p->mouse_data.flag = flag;
    p->mouse_data.new_data = 1;
  }
}

char*
refresh_lines(void)
{
  static char buff[90 * 20] = {};
  static int lines_once = 0;
  if (0 == lines_once)
  {
    lines_once = 1;
    for (int k, j = 0; j < 18; j++)
    {
      for (k = 0; k < 90; k++)
      {
        buff[j * 90 + k] = ' ';
      }
      buff[j * 80 + k] = '\n';
    }
  }
  printf("%s",buff);
  return buff;
}
