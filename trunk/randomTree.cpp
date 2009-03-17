#include "features.h"

#include "randomTree.h"


RandomTree::RandomTree(CvMat *examples, CvMat *imageTypes){


	// 1. create type mask
	CvMat *var_type = cvCreateMat( examples->cols + 1, 1, CV_8U );
	cvSet( var_type, cvScalarAll(CV_VAR_ORDERED) );
	cvSetReal1D( var_type, examples->cols, CV_VAR_CATEGORICAL );


	// 3. train classifier
	forest.train( examples, CV_ROW_SAMPLE, imageTypes, 0, 0, var_type, 0,
	CvRTParams(10,10,0,false,15,0,true,4,100,0.01f,CV_TERMCRIT_ITER));



}

RandomTree::RandomTree(const char *filename){

	forest.load( filename );

	if( forest.get_tree_count() == 0 ){

		printf( "Could not read the classifier %s\n", filename );


	}else{
		
		printf( "The classifier %s is loaded.\n", filename );
  }   

}

Features::ImageType RandomTree::classify(CvMat *imageData, double &percent){




	int pred = (int)forest.predict(imageData);

	if(pred != 5){
		std::cout << pred << std::endl;
		percent = 0.1;	

	}
	return Features::ImageType(pred);


}

RandomTree::~RandomTree(){
	
}












/*







0084 static
00085 int build_rtrees_classifier( char* data_filename,
00086     char* filename_to_save, char* filename_to_load )
00087 {
00088     CvMat* data = 0;
00089     CvMat* responses = 0;
00090     CvMat* var_type = 0;
00091     CvMat* sample_idx = 0;
00092 

00127 

00148     }
00149 
00150     // compute prediction error on train and test data
00151     for( i = 0; i < nsamples_all; i++ )
00152     {
00153         double r;
00154         CvMat sample;
00155         cvGetRow( data, &sample, i );
00156 
00157         r = forest.predict( &sample );
00158         r = fabs((double)r - responses->data.fl[i]) <= FLT_EPSILON ? 1 : 0;
00159 
00160         if( i < ntrain_samples )
00161             train_hr += r;
00162         else
00163             test_hr += r;
00164     }
00165 
00166     test_hr /= (double)(nsamples_all-ntrain_samples);
00167     train_hr /= (double)ntrain_samples;
00168     printf( "Recognition rate: train = %.1f%%, test = %.1f%%\n",
00169             train_hr*100., test_hr*100. );
00170 
00171     printf( "Number of trees: %d\n", forest.get_tree_count() );
00172 
00173     // Print variable importance
00174     var_importance = (CvMat*)forest.get_var_importance();
00175     if( var_importance )
00176     {
00177         double rt_imp_sum = cvSum( var_importance ).val[0];
00178         printf("var#\timportance (in %%):\n");
00179         for( i = 0; i < var_importance->cols; i++ )
00180             printf( "%-2d\t%-4.1f\n", i,
00181             100.f*var_importance->data.fl[i]/rt_imp_sum);
00182     }
00183 
00184     //Print some proximitites
00185     printf( "Proximities between some samples corresponding to the letter 'T':\n" );
00186     {
00187         CvMat sample1, sample2;
00188         const int pairs[][2] = {{0,103}, {0,106}, {106,103}, {-1,-1}};
00189 
00190         for( i = 0; pairs[i][0] >= 0; i++ )
00191         {
00192             cvGetRow( data, &sample1, pairs[i][0] );
00193             cvGetRow( data, &sample2, pairs[i][1] );
00194             printf( "proximity(%d,%d) = %.1f%%\n", pairs[i][0], pairs[i][1],
00195                 forest.get_proximity( &sample1, &sample2 )*100. );
00196         }
00197     }
00198 
00199     // Save Random Trees classifier to file if needed
00200     if( filename_to_save )
00201         forest.save( filename_to_save );
00202 
00203     cvReleaseMat( &sample_idx );
00204     cvReleaseMat( &var_type );
00205     cvReleaseMat( &data );
00206     cvReleaseMat( &responses );
00207 
00208     return 0;
00209 }
00210 





*/




































