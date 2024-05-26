#ifndef FILTER_PLUGIN_H
#define FILTER_PLUGIN_H


#include <common/plugins/interfaces/filter_plugin.h>
//#include "../edit_quality/common/transferfunction.h"
#include <vcg/complex/algorithms/stat.h> 
//#include "../edit_quality/common/meshmethods.h"
#include <QObject>

using namespace vcg;
using namespace std;


class Plugin : public QObject, public FilterPlugin {
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)


private:
	bool AreAllFacesTriangles(CMeshO& mesh);
	double CalculateTriangleArea(CMeshO::FaceIterator fi);
	double CalculateDistance(const vcg::Point3f& p1, const vcg::Point3f& p2);
	double CalculateMinMaxHeightsRatio(CMeshO::FaceIterator fi);
	double CalculateMaxMinSideRatio(CMeshO::FaceIterator fi);
	double CalculateCircleRadii(CMeshO::FaceIterator fi);
	bool isNegativeNaN(double x);
	double CalculateMinMaxAngles(CMeshO::FaceIterator fi);
	Color4b InterpolateColor(const vcg::Color4b& colorStart, const vcg::Color4b& colorEnd, double factor);
	Color4b GetColorForValue(double value, double min, double optimal, double max);
	Color4b AverageColors(const vcg::Color4b& color1, const vcg::Color4b& color2);
	void BlendFaceColor(CMeshO::FaceIterator fi, const vcg::Color4b& blendColor);
	

	struct
	{
		double sum;
		int counter;
		double min;
		double  max;
		map<CMeshO::FaceIterator, double> map;
	} data_struct;

	void FP_FIRST_VIS_Apply(CMeshO &mesh, int metric, int averageOfAll_or_UseRanges, bool mixColors);


	double computeAngleUsingCosineLaw(double a, double b, double c);
	void FP_SECOND_VIS_Apply(CMeshO &mesh, int mixColorsMode);

public:

    enum{
		FP_FIRST_VIS,
		FP_SECOND_VIS
    };

    Plugin(); 
    QString pluginName() const;  
    virtual QString filterName(ActionIDType filter) const; 
    QString pythonFilterName(ActionIDType f) const; 
    virtual QString filterInfo(ActionIDType filter) const;  
	virtual RichParameterList
	initParameterList(const QAction* a, const MeshDocument& md); 
    std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);  
    
	FilterArity filterArity(const QAction *act) const
	{
		return FilterPlugin::SINGLE_MESH;
	}                                                           
	virtual FilterClass getClass(const QAction *a) const;  
	QString filterScriptFunctionName(ActionIDType filterID); 
	virtual int getRequirements(const QAction* filter);

	virtual int getPreConditions(const QAction *filter) const;
	virtual int postCondition(const QAction *filter) const;
	

};

#endif // FILTER_PLUGIN_H
