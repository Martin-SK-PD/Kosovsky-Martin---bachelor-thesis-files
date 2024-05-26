#include "plugin.h"

#include <common/plugins/interfaces/filter_plugin.h>
#include <vcg/complex/algorithms/update/color.h>
#include <vcg/complex/complex.h>
#include <vcg/complex/base.h>
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>
#include <QObject>
#include <time.h>
#include <math.h>
#include <time.h>
#include <cstdlib> 
#include <vcg/space/color4.h>
#include <vcg/space/colormap.h>
#include <vcg/math/histogram.h>
#include <vcg/math/perlin_noise.h>
#include <vcg/math/random_generator.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/complex.h>
#include <vcg/complex/allocate.h>
#include <limits>



using namespace std;
using namespace tri;
using namespace vcg;




Plugin::Plugin()
{

    typeList ={
		FP_FIRST_VIS,
		FP_SECOND_VIS
    };

    for(ActionIDType tt : types())
		actionList.push_back(new QAction(filterName(tt), this));
}


QString Plugin::pluginName() const{
	return "MetricCombVis - Quality visualization plugin";
}


QString Plugin::filterName(ActionIDType filterId) const{
	switch(filterId) {
	case FP_FIRST_VIS: return QString("MetricCombVis: First visualization");
	case FP_SECOND_VIS: return QString("MetricCombVis: Second visualization");
	default : assert(0); return QString();
	}
}


QString Plugin::pythonFilterName(ActionIDType f) const{
	switch(f) {
	case FP_FIRST_VIS: return QString("");
	case FP_SECOND_VIS: return QString("");
	default : assert(0); return QString();
	}
}



QString Plugin::filterInfo(ActionIDType filterId) const{
	switch(filterId) {
	case FP_FIRST_VIS: return tr("This function allows users to apply a combination of one to five quality metrics to a mesh.<br>"
								"The user also has the option to set how colors will be mapped to individual faces.");
	case FP_SECOND_VIS: return tr("This function allows users to apply colors to vertices according to the average of the angles"
								  " that are around the vertex, or according to the maximum deviation from the optimal, 60° angle.<br>"
								  "Before applying or previewing this function, enable option of showing vertices, and optionally also "
								 "option of showing edges of the mesh in the toolbar.<br>"
								  "After applying or previewing the function, "
								  "set the color of the faces in the side panel to 'Mesh' or 'User-Def'.<br>"
								 "In the side panel, you can also set the size of the points that represent the vertices.");
	default : assert(0);
	}
	return "";
}


RichParameterList Plugin::initParameterList(const QAction* a, const MeshDocument& md)
{
	RichParameterList parlst;
	switch (ID(a)) {
	case FP_FIRST_VIS: {
		QStringList metrics;
		metrics.push_back("inradius/circumradius");
		metrics.push_back("Area");
		metrics.push_back("MinAndMaxAngle");
		metrics.push_back("MaxMinSideRatio");
		metrics.push_back("MinMaxHeights");

		parlst.addParam(RichEnum(
			"metric1",0,metrics,tr("Metric1:"),	tr("Choose a metric to compute triangle quality.")));

		metrics.push_back("None");
		parlst.addParam(RichEnum(
			"metric2",5,metrics,tr("Metric2:"),	tr("Choose a metric to compute triangle quality.")));
		parlst.addParam(RichEnum(
			"metric3",5,metrics,tr("Metric3:"),	tr("Choose a metric to compute triangle quality.")));
		parlst.addParam(RichEnum(
			"metric4",5,metrics,tr("Metric4:"),	tr("Choose a metric to compute triangle quality.")));
		parlst.addParam(RichEnum(
			"metric5",5,metrics,tr("Metric5:"),	tr("Choose a metric to compute triangle quality.")));

		QStringList mix;
		mix.push_back("Use metric specific values");
		mix.push_back("Use range values and average value as optimal");
		mix.push_back("Use range values and middle middle value of range as optimal");
		parlst.addParam(RichEnum(
			"colorMixFaktor",0,mix,tr("Color Distribution:"),tr("Choose a color distribution method.<br>"
			   "The first option uses the optimal values ​​of the given visualization.<br>"
			   "The second option uses the range of calculated values ​​and uses the average "
			   "of all values ​​as the optimal value.<br>"
			   "The third option uses the range of calculated values ​​and uses the average of "
			   "the min and max values ​​as the optimal value.")));
		break;
	}
	case FP_SECOND_VIS: {
		QStringList mix1;
		mix1.push_back("Use average of vertex angles colors");
		mix1.push_back("Use max diference from optimal angle");

		parlst.addParam(RichEnum(
			"colorMixFaktor1",0,mix1,tr("Color Distribution:"),tr("Choose a color distribution method.")));
		break;
	}
		default: break;
	}
	return parlst;
}


FilterPlugin::FilterClass Plugin::getClass(const QAction *a) const{
	switch(ID(a))
	{
	case FP_FIRST_VIS: return FilterPlugin::Quality;
	case FP_SECOND_VIS: return FilterPlugin::Quality;
	default : assert(0);  return  FilterPlugin::Generic;
	}
}



QString Plugin::filterScriptFunctionName( ActionIDType filterID )
{
	switch(filterID)
	{
	case FP_FIRST_VIS: return QString("First Vis");
	case FP_SECOND_VIS: return QString("Second Vis");
	default : assert(0);
	}
	return NULL;
}



bool Plugin::AreAllFacesTriangles(CMeshO& mesh)
{
	int faceIndex = 0; // To keep track of the face index for detailed debugging.
	
	for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end();
		++fi, ++faceIndex) {
		if (!(*fi).IsD()) { // Check if the face is not deleted
				if ((*fi).VN() != 3) {
					std::cout << "Non-triangular face at index " << faceIndex
							  << " with vertex count: " << (*fi).VN() << std::endl;
					return false; // Found a face that is not a triangle
				}

				// Check if all vertices of the triangle are distinct
				if ((*fi).V(0) == (*fi).V(1) || (*fi).V(1) == (*fi).V(2) ||
					(*fi).V(2) == (*fi).V(0)) {
					std::cout << "Degenerate triangle at index " << faceIndex
							  << " (coincident vertices)." << std::endl;
					return false;
				}
		}
	}
	
	return true; // All faces are triangles
}


// Function to calculate the area of a triangle 
double Plugin::CalculateTriangleArea(CMeshO::FaceIterator fi)
{
	Point3f & v1 = (*fi).V(0)->P();
	Point3f & v2 = (*fi).V(1)->P();
	Point3f & v3 = (*fi).V(2)->P();
	Point3f edge1 = v2 - v1;
	Point3f edge2 = v3 - v1;
	Point3f cross_product = edge1 ^ edge2; // Cross product of edge1 and edge2
	double area = 0.5 * cross_product.Norm(); // Norm of the cross product gives twice the area of the triangle
	return area;
}


// Manual calculation of the Euclidean distance between two 3D points
double Plugin::CalculateDistance(const vcg::Point3f& p1, const vcg::Point3f& p2)
{
	double dx = p2.X() - p1.X();
	double dy = p2.Y() - p1.Y();
	double dz = p2.Z() - p1.Z();
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}



// Function to calculate and return ratio of minimum and maximum height in a triangle
double Plugin::CalculateMinMaxHeightsRatio(CMeshO::FaceIterator fi)
{
	if (!(*fi).IsD()) {
		vcg::Point3f v1 = (*fi).V(0)->P();
		vcg::Point3f v2 = (*fi).V(1)->P();
		vcg::Point3f v3 = (*fi).V(2)->P();

		double area = CalculateTriangleArea(fi);

		// Calculate the length of each side of the triangle manually
		double lengthA = CalculateDistance(v2, v3); // Length of side BC
		double lengthB = CalculateDistance(v1, v3); // Length of side AC
		double lengthC = CalculateDistance(v1, v2); // Length of side AB

		// Calculate height from each vertex to the opposite side
		double heightA = (2 * area) / lengthA; // Height from vertex A to side BC
		double heightB = (2 * area) / lengthB; // Height from vertex B to side AC
		double heightC = (2 * area) / lengthC; // Height from vertex C to side AB

		// Find min and max heights
		double minHeight = std::min({heightA, heightB, heightC});
		double maxHeight = std::max({heightA, heightB, heightC});

		double ratio = minHeight / maxHeight;

		if (isnan(ratio)) {
			return 0.0;
		}

		return ratio;
	
	}

	return -1.0; // Return invalid values if the face is deleted or another error occurs
}



// Function to calculate and return the ratio of the maximum length side to the minimum length side
// in a triangle
double Plugin::CalculateMaxMinSideRatio(CMeshO::FaceIterator fi)
{
	if (!(*fi).IsD()) {
		vcg::Point3f v1 = (*fi).V(0)->P();
		vcg::Point3f v2 = (*fi).V(1)->P();
		vcg::Point3f v3 = (*fi).V(2)->P();

		// Calculate the length of each side of the triangle
		double lengthA = CalculateDistance(v1, v2);
		double lengthB = CalculateDistance(v2, v3);
		double lengthC = CalculateDistance(v3, v1);

		// Find the maximum and minimum side lengths
		double maxLength = std::max({lengthA, lengthB, lengthC});
		double minLength = std::min({lengthA, lengthB, lengthC});

		// Compute and return the ratio of the maximum length to the minimum length
		if (minLength > 0) { // Ensure no division by zero
			return maxLength / minLength;
		}
		else {
			0; // Handle cases where minimum length is zero
		}
	}

	return -1.0; // Return an invalid value if the face is deleted or another error occurs
}


// Function to compute ratio of the radius of circumscribed and inscribed circles
double Plugin::CalculateCircleRadii(CMeshO::FaceIterator fi)
{
	if (!(*fi).IsD()) {
		vcg::Point3f v1 = (*fi).V(0)->P();
		vcg::Point3f v2 = (*fi).V(1)->P();
		vcg::Point3f v3 = (*fi).V(2)->P();

		double a    = CalculateDistance(v1, v2);
		double b    = CalculateDistance(v2, v3);
		double c    = CalculateDistance(v3, v1);
		double area = CalculateTriangleArea(fi);
		double s    = (a + b + c) / 2.0; // Semi-perimeter

		if (area == 0 || s == 0) {
			return 0; 
		}

		double circumRadius = (a * b * c) / (4.0 * area);
		double inRadius     = area / s;

		if (circumRadius == 0) {
			return 0; // Return an invalid value if inRadius is zero to avoid division by zero
		}

		double ratio = inRadius / circumRadius;

		return ratio;
	}
	return 0; 
}



bool Plugin::isNegativeNaN(double x)
{
	if (std::isnan(x)) {
		uint64_t bits;
		std::memcpy(&bits, &x, sizeof(bits));
		return bits >> 63; // Check the sign bit
	}
	return false;
}

// Function to calculate angles in a triangle using the Law of Cosines, return angle with biggest diference to 60deg
double Plugin::CalculateMinMaxAngles(CMeshO::FaceIterator fi)
{
	if (!(*fi).IsD()) {
		vcg::Point3f v1 = (*fi).V(0)->P();
		vcg::Point3f v2 = (*fi).V(1)->P();
		vcg::Point3f v3 = (*fi).V(2)->P();

		// Lengths of the sides of the triangle
		double a = CalculateDistance(v2, v3);
		double b = CalculateDistance(v1, v3);
		double c = CalculateDistance(v1, v2);

		// Angles in radians using the Law of Cosines
		double angleA = std::acos((b * b + c * c - a * a) / (2 * b * c));
		double angleB = std::acos((a * a + c * c - b * b) / (2 * a * c));
		double angleC = std::acos((a * a + b * b - c * c) / (2 * a * b));

		// Convert radians to degrees
		double angleADeg = angleA * (180.0 / M_PI);
		double angleBDeg = angleB * (180.0 / M_PI);
		double angleCDeg = angleC * (180.0 / M_PI);

		// Find minimum and maximum angles
		double minAngle = std::min({angleADeg, angleBDeg, angleCDeg});
		double maxAngle = std::max({angleADeg, angleBDeg, angleCDeg});


		if (minAngle <= 0 || isNegativeNaN(minAngle)) {
			return 0;
		}
		if (maxAngle >= 180 || isnan(maxAngle)) {
			return 0;
		}

		if ((60 - minAngle) <= (maxAngle - 60)) {
			return maxAngle;
		}
		else {
			return minAngle;
		}
	}

	return -1; // Return invalid values if the face is deleted or another error occurs
}



// Function to interpolate between two colors based on a factor
Color4b Plugin::InterpolateColor(const vcg::Color4b& colorStart, const vcg::Color4b& colorEnd, double factor)
{
	return vcg::Color4b(
		colorStart[0] + (colorEnd[0] - colorStart[0]) * factor,
		colorStart[1] + (colorEnd[1] - colorStart[1]) * factor,
		colorStart[2] + (colorEnd[2] - colorStart[2]) * factor,
		255 // Set alpha to maximum for full opacity
	);
}

// Function to get a color based on a value, relative to a minimum, optimal, and maximum
Color4b Plugin::GetColorForValue(double value, double min, double optimal, double max)
{
	if (value < min)
		return vcg::Color4b(255, 0, 0, 255); // Red for minimum
	if (value > max)
		return vcg::Color4b(0, 0, 255, 255); // Blue for maximum

	if (value == optimal)
		return vcg::Color4b(0, 255, 0, 255); // Green for optimal

	// Compute the color interpolation based on proximity to optimal
	if (value < optimal) {
		// Interpolate between red and green
		double factor = (value - min) / (optimal - min);
		return InterpolateColor(vcg::Color4b(255, 0, 0, 255), vcg::Color4b(0, 255, 0, 255), factor);
	}
	else {
		// Interpolate between green and blue
		double factor = (value - optimal) / (max - optimal);
		return InterpolateColor(vcg::Color4b(0, 255, 0, 255), vcg::Color4b(0, 0, 255, 255), factor);
	}
}


Color4b Plugin::AverageColors(const vcg::Color4b& color1, const vcg::Color4b& color2)
{
	unsigned char red   = (color1[0] + color2[0]) / 2;
	unsigned char green = (color1[1] + color2[1]) / 2;
	unsigned char blue  = (color1[2] + color2[2]) / 2;
	unsigned char alpha =
		(color1[3] + color2[3]) / 2; // Assuming we also want to average the alpha channel

	return vcg::Color4b(red, green, blue, alpha);
}



void Plugin::BlendFaceColor(CMeshO::FaceIterator fi, const vcg::Color4b& blendColor)
{
	if (!(*fi).IsD()) {                      // Check if the face is not deleted
		vcg::Color4b currentColor = fi->C(); // Get the current color of the face
		vcg::Color4b newColor     = AverageColors(currentColor, blendColor);
		fi->C()                   = newColor; // Set the new averaged color to the face
	}
}



//function witch will aplly first visualization with specifict inputs
void Plugin::FP_FIRST_VIS_Apply(CMeshO &mesh, int metric, int averageOfSpecific_or_UseRanges, bool mixColors){
	
	
	if (averageOfSpecific_or_UseRanges == 0) {


		switch (metric) {
		case 0:{  //radii
			
			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
				if (!(*fi).IsD() && CalculateTriangleArea(fi)) {

					double a = CalculateCircleRadii(fi);

					if (mixColors) {
						Color4b new_color = GetColorForValue(a, 0, 0.6, 1);
						BlendFaceColor(fi, new_color);
					}
					else {
						(*fi).C() = GetColorForValue(a, 0, 0.6, 1);
					}
				}
			}

		} break;


		case 1: {// area
			
			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {

				double area = 0;
				if (!(*fi).IsD() && ( area = CalculateTriangleArea(fi)) != 0) {

					//double area = CalculateTriangleArea(fi);
					data_struct.sum += area;
					data_struct.counter++;
					if (area > data_struct.max)
						data_struct.max = area;
					if (area < data_struct.min)
						data_struct.min = area;
					data_struct.map[fi] = area;
				}
			}

			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
				if (!(*fi).IsD() && data_struct.map.find(fi)!= data_struct.map.end() ) {

					if (mixColors) {
						Color4b newColor = GetColorForValue(	
						data_struct.map.at(fi),data_struct.min , data_struct.sum / data_struct.counter, data_struct.max );
						BlendFaceColor(fi, newColor);
					}
					else {

						(*fi).C() = GetColorForValue(	
						data_struct.map.at(fi),data_struct.min , data_struct.sum / data_struct.counter, data_struct.max );
					}

				}
			}

		} break;


		case 2: {  // min max angle
			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
				if (!(*fi).IsD() && CalculateTriangleArea(fi) != 0) {

					double angle = CalculateMinMaxAngles(fi); 
					
					if (mixColors) {
						Color4b newColor = GetColorForValue(angle, 0, 60, 180);
						BlendFaceColor(fi, newColor);
					}
					else {
						(*fi).C() = GetColorForValue(angle, 0, 60, 180);
					}
				}
			}

		} break;


		case 3: { // MaxMinSideRatio

			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
				if (!(*fi).IsD() && CalculateTriangleArea(fi) !=0 ) {
					double ratio = CalculateMaxMinSideRatio(fi);

					if (mixColors) {
						Color4b new_color = GetColorForValue(ratio, 0, 1, 3);
						BlendFaceColor(fi, new_color);
					}
					else {
						(*fi).C() = GetColorForValue(ratio, 0, 1, 3);
					}
				}
			}

		} break;


		case 4: { // MinMaxHeights

			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
				if (!(*fi).IsD() && CalculateTriangleArea(fi) !=0) {
					
					
					double ratio = CalculateMinMaxHeightsRatio(fi);

					if (mixColors) {
						Color4b newColor = GetColorForValue(ratio, 0.25, 1, 2);
						BlendFaceColor(fi, newColor);
					}
					else {
						(*fi).C() = GetColorForValue(ratio, 0.25, 1, 2);
					}
				}
			}

		} break;

		case 5: return;

		default: break;
		}

	}
	else if (averageOfSpecific_or_UseRanges == 1 || averageOfSpecific_or_UseRanges == 2) {

		switch (metric) {
		case 0: { //radii
			
			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
				if (!(*fi).IsD() && CalculateTriangleArea(fi)!=0) {

					double ratio = CalculateCircleRadii(fi);

					if (mixColors) {
						double newData      = data_struct.map.at(fi) + ratio;
						data_struct.map[fi] = newData;
					}
					else {
						data_struct.map[fi] = ratio;
					}
				}
			}


		} break;

		case 1: { //area
	
			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {

				double area = 0;
				if (!(*fi).IsD() && (area = CalculateTriangleArea(fi))!=0) {

					//double area = CalculateTriangleArea(fi);
					if (mixColors) {
						double newData      = data_struct.map.at(fi) + area;
						data_struct.map[fi] = newData;
					}
					else {
						data_struct.map[fi] = area;
					}

				}
			}


		} break;

		case 2: {// min max angle
		
			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
				if (!(*fi).IsD() && CalculateTriangleArea(fi)!=0) {

					double angle = CalculateMinMaxAngles(fi); // first = min, second = max
					
					if (mixColors) {
						double newData = data_struct.map.at(fi) + angle;
						data_struct.map[fi] = newData;
					}
					else {
						data_struct.map[fi] = angle;
					}
				}
			}


		} break;

		case 3: { // MaxMinSideRatio
			
			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
				if (!(*fi).IsD() && CalculateTriangleArea(fi)!=0 ) {

					double ratio = CalculateMaxMinSideRatio(fi);

					if (mixColors) {
						double newData      = data_struct.map.at(fi) + ratio;
						data_struct.map[fi] = newData;
					}
					else {
						data_struct.map[fi] = ratio;
					}
				}
			}


		} break;

		case 4: { // MinMaxHeights
			
			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
				if (!(*fi).IsD() && CalculateTriangleArea(fi)!= 0) {

					double ratio = CalculateMinMaxHeightsRatio(fi);

					if (mixColors) {
						double newData      = data_struct.map.at(fi) + ratio;
						data_struct.map[fi] = newData;
					}
					else {
						data_struct.map[fi] = ratio;
					}
				}
			}

		} break;

		case 5: {
			return;
		}

		default: break;
		}

	}
	else { 
		throw MLException("Bad variable value"); 
	}
	
}



double Plugin::computeAngleUsingCosineLaw(double a, double b, double c)
{
	double cosC = (a * a + b * b - c * c) / (2.0 * a * b);
	return std::acos(cosC) * (180.0 / M_PI); // Return angle in radians
}


//function whitch apply second visualization
void Plugin::FP_SECOND_VIS_Apply(CMeshO& mesh, int mixColorsMode) {

	
	if (mixColorsMode == 0) { //Use average of vertex angles;

		map<CMeshO::VertexPointer, Color4b> map;

		for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
			if (!(*fi).IsD() && CalculateTriangleArea(fi)!=0) {
				CMeshO::VertexPointer v0 = (*fi).V(0);
				CMeshO::VertexPointer v1 = (*fi).V(1);
				CMeshO::VertexPointer v2 = (*fi).V(2);

				double a = CalculateDistance(v1->P(), v2->P());
				double b = CalculateDistance(v0->P(), v2->P());
				double c = CalculateDistance(v0->P(), v1->P());

				double  a0 = computeAngleUsingCosineLaw(b, c, a);
				double  a1 = computeAngleUsingCosineLaw(c, a, b);
				double  a2 = computeAngleUsingCosineLaw(a, b, c);


				Color4b color = GetColorForValue(a0, 0, 60, 180);
				if (map.find(v0) != map.end()) {
					map[v0] = AverageColors(color, map.at(v0));
				}
				else {
					map[v0] = color;
				}

				color = GetColorForValue(a1, 0, 60, 180);
				if (map.find(v1) != map.end()) {
					map[v1] = AverageColors(color, map.at(v1));
				}
				else {
					map[v1] = color;
				}

				color = GetColorForValue(a2, 0, 60, 180);
				if (map.find(v2) != map.end()) {
					map[v2] = AverageColors(color, map.at(v2));
				}
				else {
					map[v2] = color;
				}
				

			}
		}


		for (CMeshO::VertexIterator vi = mesh.vert.begin(); vi != mesh.vert.end(); ++vi) {
			if (!(*vi).IsD()) {
				try {
					(*vi).C() = map.at(&*vi);
				}
				catch (const std::exception& e) {
				}
			}
		}


	}
	else if (mixColorsMode == 1) {




		map<CMeshO::VertexPointer, double> map; 

		for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
			if (!(*fi).IsD() && CalculateTriangleArea(fi)!=0) {
				CMeshO::VertexPointer v0 = (*fi).V(0);
				CMeshO::VertexPointer v1 = (*fi).V(1);
				CMeshO::VertexPointer v2 = (*fi).V(2);

				double a = CalculateDistance(v1->P(), v2->P());
				double b = CalculateDistance(v0->P(), v2->P());
				double c = CalculateDistance(v0->P(), v1->P());

				double a0 = computeAngleUsingCosineLaw(b, c, a);
				double a1 = computeAngleUsingCosineLaw(c, a, b);
				double a2 = computeAngleUsingCosineLaw(a, b, c);


				if (map.find(v0) != map.end()) {

					double angleInMap = map.at(v0);
					double dif1 = 0, dif2 = 0;
					if (angleInMap > 60) {
						dif1 = angleInMap - 60;
					}
					else {
						dif1 = 60 - angleInMap;
					}

					if (a0 > 60) {
						dif2 =  a0 - 60;
					}
					else {
						dif2 = 60 - a0;
					}

					if (dif2 > dif1) {
						map[v0] = a0;
					}

				}
				else {
					map[v0] = a0;
				}

				if (map.find(v1) != map.end()) {

					double angleInMap = map.at(v1);
					double dif1 = 0, dif2 = 0;
					if (angleInMap > 60) {
						dif1 = angleInMap - 60;
					}
					else {
						dif1 = 60 - angleInMap;
					}

					if (a1 > 60) {
						dif2 = a1 - 60;
					}
					else {
						dif2 = 60 - a1;
					}

					if (dif2 > dif1) {
						map[v1] = a1;
					}
					
				}
				else {
					map[v1] = a1;
				}

				if (map.find(v2) != map.end()) {

					double angleInMap = map.at(v2);
					double dif1 = 0, dif2 = 0;
					if (angleInMap > 60) {
						dif1 = angleInMap - 60;
					}
					else {
						dif1 = 60 - angleInMap;
					}

					if (a2 > 60) {
						dif2 = a2 - 60;
					}
					else {
						dif2 = 60 - a2;
					}

					if (dif2 > dif1) {
						map[v2] = a2;
					}
					
				}
				else {
					map[v2] = a2;
				}
			}
		}

		for (CMeshO::VertexIterator vi = mesh.vert.begin(); vi != mesh.vert.end(); ++vi) {
			if (!(*vi).IsD()) {
				try {
					(*vi).C() = GetColorForValue( map.at(&*vi),0,60,180);
				}
				catch (const std::exception& e) {
				}
			}
		}

	}
	else{
		throw MLException("Bad variable value");
	}

}


//main filter function
std::map<std::string, QVariant> Plugin::applyFilter(
		const QAction *filter,
		const RichParameterList & par,
		MeshDocument &md,
		unsigned int& /*postConditionMask*/,
		vcg::CallBackPos *){



	MeshModel* m = md.mm();
	if (!m) return std::map<std::string, QVariant>();

    if (ID(filter) == FP_FIRST_VIS) {
		

		m->updateDataMask(MeshModel::MM_FACEFACETOPO);
		m->updateDataMask(MeshModel::MM_FACEMARK | MeshModel::MM_FACECOLOR);
		
		CMeshO& mesh = m->cm;
		RequirePerFaceColor(mesh);

		if (!AreAllFacesTriangles(mesh)) {
			throw MLException(
				"The mesh must contain only triangles");
			return std::map<std::string, QVariant>();
		}
		

		//init data_struct
		data_struct.counter = 0;
		data_struct.max     = 0;
		
		data_struct.min     = DBL_MAX;
		data_struct.sum     = DBL_MIN;
		data_struct.map.clear();


		int colorMixingMode = par.getEnum("colorMixFaktor");
		int metric = par.getEnum("metric1");
		FP_FIRST_VIS_Apply(mesh, metric, colorMixingMode, false);
		metric = par.getEnum("metric2");
		FP_FIRST_VIS_Apply(mesh, metric, colorMixingMode, true);
		metric = par.getEnum("metric3");
		FP_FIRST_VIS_Apply(mesh, metric, colorMixingMode, true);
		metric = par.getEnum("metric4");
		FP_FIRST_VIS_Apply(mesh, metric, colorMixingMode, true);
		metric = par.getEnum("metric5");
		FP_FIRST_VIS_Apply(mesh, metric, colorMixingMode, true);


		

		if (colorMixingMode == 1 || colorMixingMode == 2) {
		
			//finding min max values and 
			for (auto i = data_struct.map.begin(); i != data_struct.map.end(); i++) {
				data_struct.counter++;
				double value = i->second;

				data_struct.sum += value;
				if (value > data_struct.max)
					data_struct.max = value;
				if (value < data_struct.min)
					data_struct.min = value;
		
			}


			//final coloring
			for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
				if (!(*fi).IsD()) {

					switch (colorMixingMode) {
					case 1: {
						(*fi).C() =
						GetColorForValue(data_struct.map.at(fi), data_struct.min, data_struct.sum/data_struct.counter, data_struct.max);
					} break;
					case 2: {
						(*fi).C() = GetColorForValue(
							data_struct.map.at(fi),	data_struct.min,
							(data_struct.min+data_struct.max)/2,data_struct.max);
					} break;
					default: break;
					}

					
				}
			}

		}

		

		//reset data_struct
		data_struct.sum     = 0;
		data_struct.counter = 0;
		data_struct.min		= DBL_MAX;
		data_struct.max     = DBL_MIN;
		data_struct.map.clear();

    }
	else 
    if (ID(filter) == FP_SECOND_VIS) {


		MeshModel* m = md.mm();
		m->updateDataMask( MeshModel::MM_VERTCOLOR);
		CMeshO& mesh = m->cm;
		RequirePerVertexColor(mesh);
		
		if (!AreAllFacesTriangles(mesh)) {
			throw MLException("The mesh must contain only triangles");
			return std::map<std::string, QVariant>();
		}
		int colorMixingMode = par.getEnum("colorMixFaktor1");
		FP_SECOND_VIS_Apply(mesh, colorMixingMode);

	}
	else {
		wrongActionCalled(filter);
	}
	return std::map<std::string, QVariant>();


}

int Plugin::postCondition(const QAction *filter) const
{
	switch (ID(filter)) {
	case FP_FIRST_VIS: return MeshModel::MM_FACECOLOR | MeshModel::MM_FACEQUALITY;
	case FP_SECOND_VIS: return MeshModel::MM_VERTCOLOR;
	default: assert(0);
	}
	return MeshModel::MM_NONE;
}


int Plugin::getRequirements(const QAction*)
{
	return MeshModel::MM_NONE;
}



int Plugin::getPreConditions(const QAction *filter) const
{
	switch (ID(filter)) {
	case FP_FIRST_VIS:
	case FP_SECOND_VIS: return MeshModel::MM_NONE;
	default: break;
	return MeshModel::MM_NONE;
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(Plugin)
