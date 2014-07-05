/*
 * @file spot_light-inl.h 
 * @Brief template function implemantation of  Spotlight class for OpenGL.
 * @author Wei Chen
 * 
 * This file is part of Physika, a versatile physics simulation library.
 * Copyright (C) 2013 Physika Group.
 *
 * This Source Code Form is subject to the terms of the GNU General Public License v2.0. 
 * If a copy of the GPL was not distributed with this file, you can obtain one at:
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#ifndef PHYSIKA_GUI_LIGHT_SPOT_LIGHT_INL_H_
#define PHYSIKA_GUI_LIGHT_SPOT_LIGHT_INL_H_

namespace Physika{

template<typename Scalar>
void SpotLight::setSpotExponent(Scalar exponent)
{
    openGLLight(this->light_id_, GL_SPOT_EXPONENT, exponent);
}

template<typename Scalar>
Scalar SpotLight::spotExponent()const
{
    float exponent;
    glGetlightfv(this->light_id_, GL_SPOT_EXPONENT, &exponent );
    return (Scalar) exponent;
}

template<typename Scalar>
void SpotLight::setSpotCutoff(Scalar cutoff)
{
    openGLLight(this->light_id_, GL_SPOT_CUTOFF, cutoff);
}

template<typename Scalar>
Scalar SpotLight::spotCutoff()const
{
    float cutoff;
    glGetlightfv(this->light_id_, GL_SPOT_CUTOFF, &cutoff );
    return (Scalar) cutoff;
}

template<typename Scalar>
void SpotLight::setSpotDirection(const Vector<Scalar,3>& direction)
{
    openGLLight(this->light_id_, GL_SPOT_DIRECTION, direction)
}

template<typename Scalar>
Vector<Scalar,3> SpotLight::spotDirection()const
{
    float[3] direction;
    glGetLightfv(this->light_id_, GL_SPOT_DIRECTION, direction);
    return Vector<Scalar,3>(direction[0], direction[1], direction[2]);
}

} //end of namespace Physika

#endif //PHYSIKA_GUI_LIGHT_SPOT_LIGHT_INL_H_