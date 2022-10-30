#include<algorithm>
#include<iostream>
#include<cstdio>
#include<string>

#include"Vec3.h"
#include"OBB.h"

// writes a wavefront .obj file for a given obb
void write_obb( OBB &obb, const char *filename ){
	int quad[][4] = {
		{ 0, 3, 2, 1 },
		{ 4, 5, 6, 7 },
		{ 0, 4, 7, 3 },
		{ 1, 2, 6, 5 },
		{ 0, 1, 5, 4 },
		{ 2, 3, 7, 6 }
	};
	Vec3 pnt[8];
	FILE *fp = fopen( filename, "w" );
	obb.get_bounding_box( pnt );
	for( int i=0; i<8; i++ ){
		fprintf( fp, "v %f %f %f\n", pnt[i].x, pnt[i].y, pnt[i].z );
	}
	for( int i=0; i<6; i++ ){
		fprintf( fp, "f %d %d %d %d\n", quad[i][0]+1, quad[i][1]+1, quad[i][2]+1, quad[i][3]+1 );
	}
	fclose(fp);
}

// parses the 1-based vertex id from a wavefront .obj file.  
int parse_vertex_id( const char *c ){
	int vid;
	std::string s = c;
	std::replace( s.begin(), s.end(), '/', ' ' );
	sscanf( s.c_str(), "%d", &vid );
	return vid;
}

int main( int argc, char **argv ){
	Vec3 p;
	char line[1024], token[3][1024];
	std::vector<Vec3>	pnts;
	std::vector<int>    tris;

	// print out usage information if no command
	// line arguments are provided
	if( argc == 0 ){
		std::cout << "usage: " << argv[0] << "input.obj" << std::endl;
		return 0;
	}

	// read in a wavefront .obj file from the
	// first command-line argument
	FILE *fp = fopen( argv[1], "r" );
	while( fgets( line, 1024, fp ) ){
		if( line[0] == 'v' ){
			// vertex line, scan in the point coordinates
			// and add the point to the vertex array
			sscanf( line, "%*s%lf%lf%lf", &p.x, &p.y, &p.z );
			pnts.push_back( p );
		} else if( line[0] == 'f' ){
			// facet line, scan in the tokens and then parse
			// add the vertex ids to the triangle index list
			sscanf( line, "%*s%s%s%s", token[0], token[1], token[2] );
			tris.push_back( parse_vertex_id( token[0] )-1 );
			tris.push_back( parse_vertex_id( token[1] )-1 );
			tris.push_back( parse_vertex_id( token[2] )-1 );
		}
	}
	fclose(fp);

	// define 3 OBB objects, and build one using just
	// the model points, one using the model triangles
	// and one using the convex hull of the model
	OBB obb_pnts, obb_tris, obb_hull;
	obb_pnts.build_from_points( pnts );
	obb_tris.build_from_triangles( pnts, tris );
	obb_hull.build_from_convex_hull( pnts );

	// print the volume of the fitted boxes, since all
	// OBBs contain the input, smaller volumes indicate
	// a tighter fit
	std::cout << "obb points volume: " << obb_pnts.volume() << std::endl;
	std::cout << "obb tris volume: " << obb_tris.volume() << std::endl;
	std::cout << "obb convex hull volume: " << obb_hull.volume() << std::endl;

	// write the output as obj files
	write_obb( obb_pnts, "obb_points.obj" );
	write_obb( obb_tris, "obb_triangles.obj" );
	write_obb( obb_hull, "obb_hull.obj" );


	return 0;
}