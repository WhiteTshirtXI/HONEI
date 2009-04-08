/* vim: set sw=4 sts=4 et nofoldenable : */

/*
 * Copyright (c) 2009 Markus Geveler <apryde@gmx.de>
 * Copyright (c) 2009 Dirk Ribbrock <dirk.ribbrock@uni-dortmund.de>
 *
 * This file is part of HONEI. HONEI is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * HONEI is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef HONEI_GUARD_GL_WIDGET_HH
#define HONEI_GUARD_GL_WIDGET_HH

#include <QGLWidget>

//#include "NanoTimer.h"


class GLWidget : public QGLWidget
{
    Q_OBJECT

    public:
        GLWidget(QWidget * father = 0);
        ~GLWidget();

        QSize minimumSizeHint() const;
        QSize sizeHint() const;

    protected:
        virtual void initializeGL();
        virtual void paintGL();
        virtual void resizeGL( int width, int height );
    private:
        GLuint m_object;
        int m_xRot, m_yRot, m_zRot;
        float m_xTrans, m_yTrans, m_zTrans;
        QPoint m_lastPos;
        float m_backgroundcolor[3];
        float m_curr_rot;
        unsigned int m_numFlakeRec;
};

#endif