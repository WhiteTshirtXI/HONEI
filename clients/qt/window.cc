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


#include <QtGui>

#include <clients/qt/gl_widget.hh>
#include <clients/qt/window.hh>

Window::Window()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    glArea = new QScrollArea;
    glWidget = new GLWidget;

    glArea->setWidget(glWidget);
    glArea->setWidgetResizable(true);
    glArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    glArea->setMinimumSize(50, 50);

    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->addWidget(glArea);
    centralWidget->setLayout(mainLayout);
    setWindowTitle( "HONEI QT OpenGL client" );

    create_menu();

    resize(640,480);
}

void Window::create_menu()
{
    simulation_menu = menuBar() -> addMenu(tr("&Simulation"));
}

