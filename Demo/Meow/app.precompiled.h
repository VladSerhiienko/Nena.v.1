#pragma once

// os
#include <Windows.h>
#include <debugapi.h>
#include <cctype>
#include <ctime>
#include <string>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <set>
#include <unordered_set>

// for reign typed event
#define Safe_Delete(p) if (p) delete p, p = 0;

#include <Nena\ReignTypedEvent.hxx>
#include <Nena\ReignTypedEventHandler.hxx>

#include <ppl.h>
#include <ppltasks.h>

#define _Meow_ItemToString(i) #i

// qt 

// qt uses those defines as member function names
// thus, try to redefine to use them further

#ifdef min
	#ifndef __min
		#define __min min
	#endif
	#undef min
#endif

#ifdef max
	#ifndef __max
		#define __max max
	#endif
	#undef max
#endif

//#ifndef hook
////#define hook void
//typedef void hook;
//#else
//#error Meeeooow !
//#endif
//
//#ifndef event
//#define event
//#else
//#error Meeeooow !
//#endif

#include <QtGui>
#include <qvariant.h>
#include <qaction.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qdialog.h>
#include <qdialogbuttonbox.h>
#include <qheaderview.h>
#include <qtextedit.h>
#include <qboxlayout.h>
#include <qmenubar.h>
#include <qgroupbox.h>
#include <qgridlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qformlayout.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qtableview.h>
#include <qaction.h>
#include <qprogressbar.h>
#include <qprogressdialog.h>
#include <qtextbrowser.h>
#include <qgraphicseffect.h>
#include <qlistview.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include <qbytearray.h>
#include <qmetatype.h>
