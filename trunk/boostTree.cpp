#include "features.h"

#include "boostTree.h"


BoostTree::BoostTree(CvMat *examples, CvMat *imageTypes){


  int nsamples_all = examples->rows;
  int var_count = examples->cols;
	boost = new CvBoost;


	CvMat* new_data = cvCreateMat( nsamples_all*TYPECOUNT, var_count + 1, CV_32F );
	CvMat* new_responses = cvCreateMat( nsamples_all*TYPECOUNT, 1, CV_32S );

	for(int i = 0; i < nsamples_all; i++ )
	{
		float* data_row = (float*)(examples->data.ptr + examples->step*i);
		for(int j = 0; j < TYPECOUNT; j++ )
		{
			float* new_data_row = (float*)(new_data->data.ptr + new_data->step*(i*TYPECOUNT+j));

			for(int k = 0; k < var_count; k++ )
				 new_data_row[k] = data_row[k];

			new_data_row[var_count] = (float)j;
			new_responses->data.i[i*TYPECOUNT + j] = imageTypes->data.fl[i] == j;
		}
	}

	// 2. create type mask
	CvMat* var_type = cvCreateMat( var_count + 2, 1, CV_8U );
	cvSet( var_type, cvScalarAll(CV_VAR_ORDERED) );
	// the last indicator variable, as well
	// as the new (binary) response are categorical
	cvSetReal1D( var_type, var_count, CV_VAR_CATEGORICAL );
	cvSetReal1D( var_type, var_count+1, CV_VAR_CATEGORICAL );

	// 3. train classifier
	std::cout << "Training the classifier (may take a few minutes)..." << std::endl;
	boost->train( new_data, CV_ROW_SAMPLE, new_responses, 0, 0, var_type, 0, CvBoostParams(CvBoost::REAL, 100, 0.95, 5, false, 0 ));
	cvReleaseMat( &new_data );
	cvReleaseMat( &new_responses );
	printf("\n");

	weak_responses = cvCreateMat( 1, boost->get_weak_predictors()->total, CV_32F ); 

}

BoostTree::BoostTree(const char *filename){

	boost = new CvBoost;

	// load classifier from the specified file
	boost->load( filename );

	if( !boost->get_weak_predictors() )
	{
		printf( "Could not read the classifier %s\n", filename );

		weak_responses = NULL;

	}else{

		printf( "The classifier %s is loaded.\n", filename );

		weak_responses = cvCreateMat( 1, boost->get_weak_predictors()->total, CV_32F ); 
	}

}

Features::ImageType BoostTree::classify(CvMat *imageData, double &percent){

	double sum, max_sum = 0, tmppercent;
	int best_class = Features::OTHER;


	cvReleaseMat(&weak_responses);
	weak_responses = cvCreateMat( 1, boost->get_weak_predictors()->total, CV_32F ); 

	for(int j = 0; j < TYPECOUNT; j++ )
	{
		imageData->data.fl[Features::amountOfFeatures()] = (float)j;
		tmppercent = boost->predict( imageData, 0, weak_responses );
		sum = cvSum( weak_responses ).val[0];

		if( max_sum < sum )
		{
			percent = tmppercent;
			max_sum = sum;
			best_class = j;
		}
	}

	//std::cout << percent << std::endl;

	if(best_class != 5 && best_class != 0){
		std::cout << best_class << std::endl;
	}
 

	return Features::ImageType(best_class);


}

BoostTree::~BoostTree(){
	
	delete boost;
	cvReleaseMat(&weak_responses);

}
