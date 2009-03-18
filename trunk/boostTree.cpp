#include "features.h"

#include "boostTree.h"
#include "CfgReader.h"

BoostTree::BoostTree(CvMat *examples, CvMat *imageTypes){

	// load some basic boost tree properties
	int type = CfgReader::getInt("boostType");
	int weakCount = CfgReader::getInt("boostWeakCount");
	double weightTrimRrate = CfgReader::getDouble("boostWeightTrimRrate");
	int maxDepth = CfgReader::getInt("boostMaxDepth");
	bool useSurrogates = (bool)CfgReader::getInt("boostUseSurrogates");

	// amount of variables
	int var_count = examples->cols;

	std::cout << "Current settings: " << type << " " << weakCount << " " << weightTrimRrate << " " << maxDepth << " " << useSurrogates << std::endl;
	
	// create type mask
	CvMat* var_type = cvCreateMat( var_count + 1, 1, CV_8UC1 );
	
	for(int j=0;j<var_count;++j)
		CV_MAT_ELEM(*var_type, unsigned char, j, 0) = CV_VAR_NUMERICAL;

	// the new (binary) response are categorical
	cvSetReal1D( var_type, var_count, CV_VAR_CATEGORICAL );

	// if we want to get some stats on how well we are doing we can use a subset of the data to train and a subset to test.
	bool createGraph = (bool)CfgReader::getInt("createGraph");

	CvMat* trainData, *testData = NULL;
	CvMat* trainResponses = NULL, *testResponses = NULL;

	// create graph means we are going to test our classifier on some portion of the data. Since our data does not come in in a random
	// fashion we need to give some bits to training and some to test
	if(createGraph){

		int trainPercent = (int)(10.0 * CfgReader::getDouble("graphTrainTestRatio"));
		
		trainData = cvCreateMat( (int)(examples->rows * CfgReader::getDouble("graphTrainTestRatio")), var_count, CV_32F );
		testData = cvCreateMat( (int)(examples->rows  - trainData->rows), var_count, CV_32F );

		trainResponses = cvCreateMat( trainData->rows, 1, CV_32SC1 );
		testResponses = cvCreateMat( testData->rows, 1, CV_32SC1 );

		int trainCount = 0;
		int testCount = 0;

		// for each row give some percentage to train some to test
		for(int i = 0; i < examples->rows; i++ )
		{
			float* data_row = (float*)(examples->data.ptr + examples->step*i);
			float* new_data_row = NULL;

			// If train is not full we give it some values if i%10 is 1,2... up to train percent. If test is full just put the last few in train.
			if((i%10 < trainPercent || testCount >= testData->rows) && trainCount < trainData->rows){

				new_data_row = (float*)(trainData->data.ptr + trainData->step*trainCount);

				trainResponses->data.i[trainCount] = imageTypes->data.i[i];
				trainCount++;

			}else if(testCount < testData->rows){

				new_data_row = (float*)(testData->data.ptr + testData->step*testCount);
				testResponses->data.i[testCount] = imageTypes->data.i[i];

				testCount++;				
			}
			if(new_data_row){
				for(int k = 0; k < var_count; k++ )
					 new_data_row[k] = data_row[k];
			}
		}

	}else{

		// if we are not doing some testing of the results just make the trainData and trainResponses point to the actual data
  	trainData = examples;
  	trainResponses = imageTypes;

	}

	int nsamples = trainData->rows;

	
	for(int j = 0; j < TYPECOUNT; j++ ){

		boost.push_back(new CvBoost);

		// create a new responses matrix that contains booleans for this specific type we are classifying.
		CvMat* new_responses = cvCreateMat( nsamples, 1, CV_32SC1 );

		for(int i = 0; i < nsamples; i++ )
		{

			new_responses->data.i[i] = (int)(trainResponses->data.i[i] == j);

		}

		// train classifier
		std::cout << "Training the classifier (may take a few minutes)..." << std::endl;
		boost.at(j)->train( trainData, CV_ROW_SAMPLE, new_responses, 0, 0, var_type, 0, 
											CvBoostParams(type, weakCount, weightTrimRrate, maxDepth, useSurrogates, 0 ));

		cvReleaseMat( &new_responses );
		
		printf("\n");

		// save weak classifiers in vector for future classifications
		weak_responses.push_back(cvCreateMat( 1, boost.at(j)->get_weak_predictors()->total, CV_32F ));

	}

	cvReleaseMat( &var_type );


	// lets test the quality of the results shall we :) 
	if(createGraph && testData != NULL){

		CvMat sample;
		int goody, best_class;
		double sum, max_sum;
		int testTotalCount = 0, trainTotalCount = 0;
		int testHr = 0, trainHr = 0;

		// compute prediction error on train and test data
		// loop through images that are not of type other. Try to predict their type. Save results.
		for(int i = 0; i < testData->rows; i++ )
		{

			max_sum = -10000000;
			best_class = 5;
			
			cvGetRow( testData, &sample, i );

			if(testResponses->data.i[i] != 5){
				for(int j = 0; j < TYPECOUNT; j++ ){

					goody = (int)boost.at(j)->predict( &sample, 0, weak_responses.at(j) );
					sum = cvSum( weak_responses.at(j) ).val[0];

					if( max_sum < sum)
					{
						max_sum = sum;
						best_class = j;
					}

				}
			
				// save amount of images tested
				testTotalCount++;

				std::cout << best_class << " " << testResponses->data.i[i] << std::endl;

				// save amount of correct results
				testHr += best_class == testResponses->data.i[i];
			}
		}


		// compute prediction error on train and train data
		// loop through images that are not of type other. Try to predict their type. Save results.
		for(int i = 0; i < trainData->rows; i++ )
		{

			max_sum = -10000000;
			best_class = 5;
			
			cvGetRow( trainData, &sample, i );

			if(trainResponses->data.i[i] != 5){
				for(int j = 0; j < TYPECOUNT; j++ ){

					goody = (int)boost.at(j)->predict( &sample, 0, weak_responses.at(j) );
					sum = cvSum( weak_responses.at(j) ).val[0];

					if( max_sum < sum)
					{
						max_sum = sum;
						best_class = j;
					}

				}

				// save amount of images tested
				trainTotalCount++;

				std::cout << best_class << " " << trainResponses->data.i[i] << std::endl;

				// save amount of correct results
				trainHr += best_class == trainResponses->data.i[i];
			}
		}

		//test_hr /= (double)(nsamples_all-ntrain_samples);
		//train_hr /= (double)ntrain_samples;

		std::cout << "Testing accuracy: " << (float)testHr/(float)testTotalCount << std::endl;
		std::cout << "Training accuracy: " << (float)trainHr/(float)trainTotalCount << std::endl;

		cvReleaseMat(&trainData);
		cvReleaseMat(&testData);
		cvReleaseMat(&trainResponses);
		cvReleaseMat(&testResponses);


	}
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

Features::ImageType BoostTree::classify(CvMat *imageData, double *percent){

	double maxSum;
	int best_class = Features::OTHER, goody;


	for(int j = 0; j < TYPECOUNT; j++ )
	{

		goody = (int)boost.at(j)->predict( imageData, 0, weak_responses.at(j) );
		percent[j] = cvSum( weak_responses.at(j) ).val[0];

		if( maxSum < percent[j] && goody == 1)
		{
			maxSum = percent[j];
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
