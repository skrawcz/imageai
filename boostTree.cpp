#include "features.h"

#include "boostTree.h"


BoostTree::BoostTree(CvMat *examples, CvMat *imageTypes){


  int nsamples_all = examples->rows;
  int var_count = examples->cols;
	
	
	// 2. create type mask
	CvMat* var_type = cvCreateMat( var_count + 1, 1, CV_8UC1 );
	
	for(int j=0;j<var_count;++j)
		CV_MAT_ELEM(*var_type, unsigned char, j, 0) = CV_VAR_NUMERICAL;
		

	// as the new (binary) response are categorical
	cvSetReal1D( var_type, var_count, CV_VAR_CATEGORICAL );


	for(int j = 0; j < TYPECOUNT; j++ ){

		boost.push_back(new CvBoost);

		CvMat* new_responses = cvCreateMat( nsamples_all, 1, CV_32SC1 );

		for(int i = 0; i < nsamples_all; i++ )
		{

			new_responses->data.i[i] = (int)(imageTypes->data.i[i] == j);

		}



		// 3. train classifier
		std::cout << "Training the classifier (may take a few minutes)..." << std::endl;
		boost.at(j)->train( examples, CV_ROW_SAMPLE, new_responses, 0, 0, var_type, 0, CvBoostParams(CvBoost::REAL, 100, 0.95, 5, false, 0 ));

		cvReleaseMat( &new_responses );
		
		printf("\n");

		weak_responses.push_back(cvCreateMat( 1, boost.at(j)->get_weak_predictors()->total, CV_32F ));

	}

	cvReleaseMat( &var_type );


}

BoostTree::BoostTree(const char *filename){

	for(int j = 0; j < TYPECOUNT; j++ ){
		boost.push_back(new CvBoost);

		std::string egon(filename);
		std::stringstream out;
		out << j << ".xml";
		egon.append(out.str());

		std::cout << egon << std::endl;

		boost.at(j)->load( egon.c_str() );

		if( !boost.at(j)->get_weak_predictors() )
		{
			printf( "Could not read the classifier %s\n", filename );

			weak_responses.push_back(NULL);

		}else{

			printf( "The classifier %s is loaded.\n", filename );

			weak_responses.push_back(cvCreateMat( 1, boost.at(j)->get_weak_predictors()->total, CV_32F )); 
		}
	}

}

Features::ImageType BoostTree::classify(CvMat *imageData, double &percent){

	double sum;
	int best_class = Features::OTHER, goody;


	for(int j = 0; j < TYPECOUNT; j++ )
	{

		goody = boost.at(j)->predict( imageData, 0, weak_responses.at(j) );
		sum = cvSum( weak_responses.at(j) ).val[0];

		if( percent < sum && goody == 1)
		{
			percent = sum;
			best_class = j;
		}
	}

	return Features::ImageType(best_class);


}

BoostTree::~BoostTree(){
	
	for(int j = 0; j < TYPECOUNT; j++ ){
		delete boost.at(j);
		cvReleaseMat(&weak_responses.at(j));
	}
}
