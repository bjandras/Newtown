/*
 * This file is part of Newtown.
 *
 * Copyright (C) 2013 Borko Jandras <bjandras@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "core/border.h"
#include "base/bitmatrix.h"
#include "math/graph.h"

#include <iostream>
#include <string>

#include <QtCore>
#include <QtGui>


void run_boundary()
{
	using namespace core::border;

	QImage image("data/boundarymap.tiff");
	if (image.isNull()) return;

	Regions regions;
	findRegions(regions, image);
	qDebug() << "found" << regions.size() << "region(s)";

	QImage output(image.size(), QImage::Format_ARGB32);

	int ci = 0; // color index
	QVector<QColor> colors;
	colors << "red" << "green" << "blue" << "yellow" << "cyan" << "magenta";
	
	foreach (Label label, regions.keys())
	{
		Boundary const & boundary = regions[label];
		qDebug() << QColor(label).name() << boundary.size();

		foreach (BoundarySegment const & segment, boundary)
		{
			QColor color = colors[ci];
			ci = (ci+1) % colors.size();
			
			foreach (QPoint p, segment)
			{
				output.setPixel(p, color.rgba());
			}
		}
	}
	
	output.save("boundary.tiff");
}


void run_bitmatrix()
{
	using base::BitMatrix;

	BitMatrix mat(3, 3);
	mat.fill(false);
	qDebug() << "-- zero";
	qDebug() << mat;

	mat(0,0) = 1;
	mat(1,1) = 1;
	mat(2,2) = 1;
	qDebug() << "-- id";
	qDebug() << mat;
	qDebug() << mat(0,0);
	qDebug() << mat(0,1);
}


void run_graph()
{
	using math::Graph;

	Graph g(5);

	g.connect(1,2);
	g.connect(2,3);
	g.connect(3,4);
	g.connect(4,5);
	qDebug() << g.edges();
	qDebug() << g.allPairsShortestPaths().getPath(1,5);

	g.connect(3,5);
	qDebug() << g.allPairsShortestPaths().getPath(1,5);

	g.connect(5,2);
	qDebug() << g.allPairsShortestPaths().getPath(1,5);
}

void run_mcb()
{
	using math::Graph;

	Graph g(6);

	g.connect(1,2);
	g.connect(2,3);
	g.connect(2,4);
	g.connect(3,4);
	g.connect(4,1);

	qDebug() << g.minimumCycleBasis();

	g.connect(2,5);
	g.connect(5,3);
	qDebug() << g.minimumCycleBasis();

	g.connect(4,6);
	g.connect(5,6);
	qDebug() << g.minimumCycleBasis();
}


extern "C" int transformdemo_main(int, char **);

extern "C" int demo_main(int argc, char ** argv)
{
	std::string name = "default";

	if (argc > 1) name = argv[1];

	if (name == "boundary")
	{
		run_boundary();
	}
	else if (name == "bitmatrix")
	{
		run_bitmatrix();
	}
	else if (name == "graph")
	{
		run_graph();
	}
	else if (name == "mcb")
	{
		run_mcb();
	}
	else if (name == "transform")
	{
		return transformdemo_main(argc, argv);
	}
	else
	{
		std::cerr << "unknown demo: " << name << std::endl;
	}
	
	return 0;
}
