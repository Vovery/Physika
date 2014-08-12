/*
 * @file mpm_solid_plugin_render.cpp 
 * @brief plugin for real-time render of MPMSolid driver.
 * @author Fei Zhu
 * 
 * This file is part of Physika, a versatile physics simulation library.
 * Copyright (C) 2013 Physika Group.
 *
 * This Source Code Form is subject to the terms of the GNU General Public License v2.0. 
 * If a copy of the GPL was not distributed with this file, you can obtain one at:
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include "Physika_Core/Utilities/math_utilities.h"
#include "Physika_Core/Utilities/physika_assert.h"
#include "Physika_GUI/Glut_Window/glut_window.h"
#include "Physika_Render/Point_Render/point_render.h"
#include "Physika_Render/OpenGL_Primitives/opengl_primitives.h"
#include "Physika_Render/Color/color.h"
#include "Physika_Geometry/Cartesian_Grids/grid.h"
#include "Physika_Dynamics/Particles/solid_particle.h"
#include "Physika_Dynamics/MPM/MPM_Plugins/mpm_solid_plugin_render.h"

namespace Physika{

template <typename Scalar, int Dim>
MPMSolidPluginRender<Scalar,Dim>* MPMSolidPluginRender<Scalar,Dim>::active_instance_ = NULL;

template <typename Scalar, int Dim>
MPMSolidPluginRender<Scalar,Dim>::MPMSolidPluginRender()
    :MPMSolidPluginBase<Scalar,Dim>(),window_(NULL),pause_simulation_(true),
     simulation_finished_(false),render_particle_(true),render_grid_(true),
     render_particle_velocity_(false),render_grid_velocity_(false),
     particle_render_mode_(0),velocity_scale_(1.0),auto_capture_frame_(false)
{
    activateCurrentInstance();
}

template <typename Scalar, int Dim>
MPMSolidPluginRender<Scalar,Dim>::~MPMSolidPluginRender()
{
}


template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onBeginFrame(unsigned int frame)
{
//do nothing, because advanceFrame() is never called
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onEndFrame(unsigned int frame)
{
//do nothing, because advanceFrame() is never called
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onBeginTimeStep(Scalar time, Scalar dt)
{
   //start timer when the first frame begins
    MPMSolid<Scalar,Dim> *driver = this->driver();
    PHYSIKA_ASSERT(driver);
    Scalar frame_rate = driver->frameRate();
    Scalar cur_frame_scalar = time*frame_rate;
    unsigned int cur_frame = static_cast<unsigned int>(cur_frame_scalar);
    unsigned int start_frame = driver->getStartFrame();
    if(cur_frame_scalar-start_frame<std::numeric_limits<Scalar>::epsilon()) //begins the first frame
    {
        if(driver->isTimerEnabled())
            timer_.startTimer();
        std::cout<<"Begin Frame "<<cur_frame<<"\n";
    }
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onEndTimeStep(Scalar time, Scalar dt)
{
    //stop timer when a frame ends
    //stop simulation when maximum frame reached
    MPMSolid<Scalar,Dim> *driver = this->driver();
    PHYSIKA_ASSERT(driver);
    unsigned int max_frame = driver->getEndFrame();
    Scalar frame_rate = driver->frameRate();
    unsigned int cur_frame = static_cast<unsigned int>(time*frame_rate);
    unsigned int frame_last_step = static_cast<unsigned int>((time-dt)*frame_rate);
    if(cur_frame-frame_last_step==1) //ended a frame
    {
        std::cout<<"End Frame "<<cur_frame<<" ";
        if(driver->isTimerEnabled())
        {
            timer_.stopTimer();
            std::cout<<timer_.getElapsedTime()<<" s";
        }
        std::cout<<"\n";
        if(this->auto_capture_frame_)  //write screen to file
        {
            std::string file_name("./screen_shots/frame_");
            std::stringstream adaptor;
            adaptor<<cur_frame;
            std::string cur_frame_str;
            adaptor>>cur_frame_str;
            file_name += cur_frame_str+std::string(".png");
            this->window_->saveScreen(file_name);
        }
        if(driver->isTimerEnabled())
        {
            //start timer for next frame
            //not accurate if something happens between time steps
            if(cur_frame < max_frame)
                timer_.startTimer();
        }
    }
    if(cur_frame >= max_frame)
    {
        this->simulation_finished_ = true;
        std::cout<<"Simulation Ended.\n";
    }
}

template <typename Scalar, int Dim>
MPMSolid<Scalar,Dim>* MPMSolidPluginRender<Scalar,Dim>::driver()
{
    MPMSolid<Scalar,Dim> *driver = dynamic_cast<MPMSolid<Scalar,Dim>*>(this->driver_);
    return driver;
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::setDriver(DriverBase<Scalar> *driver)
{
    if(driver==NULL)
    {
        std::cerr<<"Error: NULL driver pointer provided to driver plugin, program abort!\n";
        std::exit(EXIT_FAILURE);
    }
    if(dynamic_cast<MPMSolid<Scalar,Dim>*>(driver)==NULL)
    {
        std::cerr<<"Error: Wrong type of driver specified, program abort!\n";
        std::exit(EXIT_FAILURE);
    }
    this->driver_ = driver;
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onRasterize()
{
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onSolveOnGrid(Scalar dt)
{
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onPerformGridCollision(Scalar dt)
{
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onPerformParticleCollision(Scalar dt)
{
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onUpdateParticleInterpolationWeight()
{
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onUpdateParticleConstitutiveModelState(Scalar dt)
{
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onUpdateParticleVelocity()
{
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::onUpdateParticlePosition(Scalar dt)
{
}

template <typename Scalar, int Dim>
GlutWindow* MPMSolidPluginRender<Scalar,Dim>::window()
{
    return window_;
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::setWindow(GlutWindow *window)
{
    if(window==NULL)
    {
        std::cerr<<"Error: NULL window pointer provided to render plugin, program abort!\n";
        std::exit(EXIT_FAILURE);
    }
    window_ = window;
    window_->setIdleFunction(MPMSolidPluginRender<Scalar,Dim>::idleFunction);
    window_->setDisplayFunction(MPMSolidPluginRender<Scalar,Dim>::displayFunction);
    window_->setKeyboardFunction(MPMSolidPluginRender<Scalar,Dim>::keyboardFunction);
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::idleFunction(void)
{
//TO DO
    PHYSIKA_ASSERT(active_instance_);
    MPMSolid<Scalar,Dim> *driver = active_instance_->driver();
    PHYSIKA_ASSERT(driver);
    Scalar dt = driver->computeTimeStep();
    if(active_instance_->pause_simulation_ == false &&
       active_instance_->simulation_finished_ == false)
    {
        driver->advanceStep(dt);
        //active_instance_->pause_simulation_ = true;
    }
    glutPostRedisplay();
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::displayFunction(void)
{
    PHYSIKA_ASSERT(active_instance_);
    GlutWindow *window = active_instance_->window_;
    PHYSIKA_ASSERT(window);
    Color<double> background_color = window->backgroundColor<double>();
    glClearColor(background_color.redChannel(), background_color.greenChannel(), background_color.blueChannel(), background_color.alphaChannel());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    window->applyCameraAndLights();

    if(active_instance_->render_particle_)
        active_instance_->renderParticles();
    if(active_instance_->render_grid_)
        active_instance_->renderGrid();
    if(active_instance_->render_particle_velocity_)
        active_instance_->renderParticleVelocity();
    if(active_instance_->render_grid_velocity_)
        active_instance_->renderGridVelocity();
    window->displayFrameRate();
    glutSwapBuffers();
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::keyboardFunction(unsigned char key, int x, int y)
{
    PHYSIKA_ASSERT(active_instance_);
    GlutWindow *window = active_instance_->window_;
    PHYSIKA_ASSERT(window);
    window->bindDefaultKeys(key,x,y);  //default key is preserved
    switch(key)
    {
    case 'p':
        active_instance_->render_particle_ = !(active_instance_->render_particle_);
        break;
    case 'g':
        active_instance_->render_grid_ = !(active_instance_->render_grid_);
        break;
    case 'P':
        active_instance_->render_particle_velocity_ = !(active_instance_->render_particle_velocity_);
        break;
    case 'G':
        active_instance_->render_grid_velocity_ = !(active_instance_->render_grid_velocity_);
        break;
    case 'i':
        active_instance_->velocity_scale_ *= 2.0;
        break;
    case 'd':
        active_instance_->velocity_scale_ /= 2.0;
        break;
    case 'm':
        active_instance_->particle_render_mode_ = 1 - active_instance_->particle_render_mode_;
        break;
    case 32: //space
        active_instance_->pause_simulation_ = !(active_instance_->pause_simulation_);
        break;
    case 'S':
        active_instance_->auto_capture_frame_ = !(active_instance_->auto_capture_frame_);
        break;
    default:
        break;
    }
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::activateCurrentInstance()
{
    active_instance_ = this;
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::renderParticles()
{
//TO DO: speed up
    PHYSIKA_ASSERT(active_instance_);
    MPMSolid<Scalar,Dim> *driver = active_instance_->driver();
    PHYSIKA_ASSERT(driver);
    const std::vector<SolidParticle<Scalar,Dim>*> &all_particles = driver->allParticles();
    Vector<Scalar,Dim> *particle_pos = new Vector<Scalar,Dim>[all_particles.size()];
    for(unsigned int i = 0; i < all_particles.size(); ++i)
        particle_pos[i] = all_particles[i]->position();
    PointRender<Scalar,Dim> point_render(particle_pos,all_particles.size());
    std::vector<Scalar> point_size(all_particles.size());
    if(active_instance_->particle_render_mode_ == 0)
        point_render.setRenderAsPoint();
    else
    {
        point_render.setRenderAsSphere();
        for(unsigned int i = 0; i < point_size.size(); ++i)
            point_size[i] = (Dim==2) ? sqrt(all_particles[i]->volume()/(4.0*PI)) : pow(all_particles[i]->volume()*3/(4*PI),1.0/3.0);
    }
    point_render.render();
    delete[] particle_pos;
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::renderGrid()
{
    PHYSIKA_ASSERT(active_instance_);
    MPMSolid<Scalar,Dim> *driver = active_instance_->driver();
    PHYSIKA_ASSERT(driver);
    const Grid<Scalar,Dim> &grid = driver->grid();
    openGLColor3(Color<Scalar>::White());
    glDisable(GL_LIGHTING);
    for(typename Grid<Scalar,Dim>::CellIterator iter = grid.cellBegin(); iter != grid.cellEnd(); ++iter)
    {  //TEST CODE, ONLY FOR 2D
        Vector<unsigned int, Dim> cell_idx = iter.cellIndex();
        Vector<Scalar,Dim> cell_min = grid.cellMinCornerNode(cell_idx);
        Vector<Scalar,Dim> cell_max = grid.cellMaxCornerNode(cell_idx);
        Vector<Scalar,Dim> node2(cell_min);
        node2[0] = cell_max[0];
        Vector<Scalar,Dim> node4(cell_min);
        node4[1] = cell_max[1];
        glBegin(GL_LINE_LOOP);
        openGLVertex(cell_min);
        openGLVertex(node2);
        openGLVertex(cell_max);
        openGLVertex(node4);
        glEnd();
    }
//TO DO: replaced with grid render
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::renderParticleVelocity()
{
    PHYSIKA_ASSERT(active_instance_);
    MPMSolid<Scalar,Dim> *driver = active_instance_->driver();
    PHYSIKA_ASSERT(driver);
    openGLColor3(Color<Scalar>::Red());
    glDisable(GL_LIGHTING);
    for(unsigned int i = 0; i < driver->particleNum(); ++i)
    {
        const SolidParticle<Scalar,Dim>& particle = driver->particle(i);
        Vector<Scalar,Dim> start = particle.position();
        Vector<Scalar,Dim> end = start + (active_instance_->velocity_scale_)*particle.velocity();
        glBegin(GL_LINES);
        openGLVertex(start);
        openGLVertex(end);
        glEnd();
    }
}

template <typename Scalar, int Dim>
void MPMSolidPluginRender<Scalar,Dim>::renderGridVelocity()
{
//TO DO
    PHYSIKA_ASSERT(active_instance_);
    MPMSolid<Scalar,Dim> *driver = active_instance_->driver();
    PHYSIKA_ASSERT(driver);
    const Grid<Scalar,Dim> &grid = driver->grid();
    openGLColor3(Color<Scalar>::Red());
    glDisable(GL_LIGHTING);
    for(typename Grid<Scalar,Dim>::NodeIterator iter = grid.nodeBegin(); iter != grid.nodeEnd(); ++iter)
    {  
        Vector<unsigned int,Dim> node_idx = iter.nodeIndex();
        Vector<Scalar,Dim> start = grid.node(node_idx);
        Vector<Scalar,Dim> end = start + (active_instance_->velocity_scale_)*(driver->gridVelocity(node_idx));
        glBegin(GL_LINES);
        openGLVertex(start);
        openGLVertex(end);
        glEnd();
    }
}

//explicit instantiations
template class MPMSolidPluginRender<float,2>;
template class MPMSolidPluginRender<float,3>;
template class MPMSolidPluginRender<double,2>;
template class MPMSolidPluginRender<double,3>;

}  //end of namespace Physika