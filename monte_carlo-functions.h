/* ------------------------------------------------------------------- */
// initialization and storing
/* ------------------------------------------------------------------- */



void CheckAndReadInput(int argc,char* argv[],
FILE** outputparam,FILE** outputdata, params* Params){
    int i;
    char name[200]; // string in which the file names are written
    int argctarget;
    char command_base[1000]="";
    strcat(command_base, "usage: ");
    strcat(command_base, argv[0] );
    strcat(command_base," output_file FinalIndex mode separation seed \n");
    argctarget = 6;  
    

    // Read the passed arguments
    if(argc!=argctarget){
        printf("%d arguments required: %d missing! \n",argctarget-1, argctarget-argc);
        printf("%s\n",command_base);
        exit(1);
    }

    i=1;
    // Open file-param, file-pos.
    sprintf(name,"%s-param",argv[i]);
    outputparam[0]=fopen(name,"w");
    
    sprintf(name,"%s-data",argv[i]);
    outputdata[0]=fopen(name,"w");

    i++;
    Params[0].FinalIndex    = (int) strtod(argv[i], NULL); i++;
    Params[0].mode          = (int) strtod(argv[i], NULL); i++;  // 0 for  horizontal, 1 for vertical
    Params[0].separation    = strtod(argv[i], NULL); i++;
    Params[0].seed          = (long long unsigned int) strtod(argv[i], NULL); i++;

    if(Params[0].mode == 0){        // Horizontal
        Params[0].width     = (double) 5.0;
        Params[0].depth     = (double) 5.0;
        Params[0].height    = (double) 1.0;
    }
    
    if(Params[0].mode == 1){        // Vertical
        Params[0].width     = (double) 5.0;
        Params[0].depth     = (double) 1.0;
        Params[0].height    = (double) 5.0;
    }

    // Maximum angle for coincident hit
    Params[0].max_theta     = (double) atan(sqrt(Params[0].width*Params[0].width + Params[0].depth*Params[0].depth)/Params[0].separation);
}




void store_params(int argc, char* argv[], FILE* outputparam, struct params Params) {
    int i;
    char command_base[1000]="";
    /*Print the command line in the parameters file*/
    strcat(command_base, "usage: ");
    strcat(command_base, argv[0]);
    strcat(command_base," output_file FinalIndex mode separation seed StoreInter \n");
    fprintf(outputparam,"%s", command_base);
    for(i=0;i<argc;i++){
        fprintf(outputparam,"%s ",argv[i]);
    }
    fprintf(outputparam,"\n");

    fprintf( outputparam, "Final index      :   %d\n",      Params.FinalIndex);
    if (Params.mode == 0) {
        fprintf( outputparam, "Mode             :   horizontal\n");
    } else {
        fprintf( outputparam, "Mode             :   vertical\n");
    }
    fprintf( outputparam, "Separation       :   %lf cm \n",    Params.separation);
    fprintf( outputparam, "seed             :   %lld\n",    Params.seed);
    fprintf( outputparam, "Max theta        :   %lf\n",    Params.max_theta);
    fprintf( outputparam, "Scint width      :   %lf\n",    Params.width);
    fprintf( outputparam, "Scint depth      :   %lf\n",    Params.depth);
    fprintf( outputparam, "Scint height     :   %lf\n",    Params.height);
    

    fflush(outputparam);
}




// void Store_Lengths(FILE* outputlengths, double* lengths, struct params Params){
//     for ( int i =0; Params.StoreInter ; i++){
//         fprintf( outputlengths, "%lf\t", lengths[i]);
//     }
//     fprintf( outputlengths, "\n");
//     fflush( outputlengths);
// }

void Store_Data(FILE* outputdata, double x, double y, double theta, double phi, double length){
    fprintf( outputdata, "%lf\t%lf\t%lf\t%lf\t%lf\n", x, y, theta, phi, length);
    fflush( outputdata);
}

/* ------------------------------------------------------------------- */
// main functions
/* ------------------------------------------------------------------- */


// Geometric Probability Density Function (PDF)
double geometric_pdf(double theta) {
    return cos(theta) * sin(theta);
}


// You can adjust this to correspond to the hypothetical physical PDF of radiation. The default is cos^2(theta).
double physical_pdf(double theta) {
    return 4 * pow(cos(theta),2);
}


double Get_random_theta(params Params) {
    // Use rejection sampling with a uniform distribution. Can be optimized; right now rejection rate ~ 1/2 (depends on theta_max)
    double threshold, candidate, pdf_value, maximum;
    
    maximum = 1.3;      // Adjust this such that: (combined geometric and physical pdf) \leq (maximum)

    do {
        threshold = maximum * genrand64_real2();
        // Create candidate theta uniformly from the possible theta range:
        candidate = Params.max_theta * genrand64_real2();
        pdf_value = geometric_pdf(candidate) * physical_pdf(candidate);
    } while (threshold > pdf_value);

    return candidate;
}



void Get_length_theta(params Params, FILE* outputdata) {
    double x, y, theta, phi, x_bot, y_bot, x_top, y_top, x_wall, y_wall, length;
    int coincident;
    coincident = 0;
    do{
        // Draw x, y position on bottom of top scintillator and phi:
        x       = Params.width * genrand64_real2();
        y       = Params.depth * genrand64_real2();
        phi     = 2*M_PI * genrand64_real2();

        // Draw theta from the distribution P(theta) \propto cos^3(theta) * sin(theta):
        theta   = Get_random_theta(Params);

        // Calculate the position at bottom:
        x_bot   = x - Params.separation * tan(theta) * cos(phi);
        y_bot   = y - Params.separation * tan(theta) * sin(phi);

        // check coincidence:
        if (0 < x_bot && x_bot < Params.width && 0 < y_bot && y_bot < Params.depth) {
            coincident = 1;
        }

    } while (coincident != 1);

    // Now we have found a coincident ray. We need to find the distance it travels through the top scintillator.
    
    // Case 1: the ray goes through the top of the scintillator
    x_top   = x + Params.height * tan(theta) * cos(phi);
    y_top   = y + Params.height * tan(theta) * sin(phi);


    // printf("x_top : %lf \t\t" , x_top);
    // printf("y_top : %lf \n", y_top);

    // printf("x : %lf \t\t" , x);
    // printf("y : %lf \n", y);
    // printf("theta : %lf \t" , theta);
    // printf("phi : %lf \n", phi);
    

    // check if went through top:
    if (0 < x_top && x_top < Params.width && 0 < y_top && y_top < Params.depth) {
        // Simple trigonometry
        length  = (double) Params.height / cos(theta);
        // printf("roof : %lf \n" , length);
    } else {    
        
        
        // Case 2: Did not go through top. This case is more tedious. Went through some side. Need to find point of entry
        // Divide into possible quadrants. Work counterclockwise in xy-plane
        
        if (x_top > Params.width) {                                         // Quadrant 1
            x_wall = Params.width;
            
            // Check if y hit wall first
            y_wall = y + tan(phi) * (x_wall - x);

            if (y_wall > Params.depth) {                                    // y hit first on the plus side
                length = fabs((Params.depth - y)/(sin(theta)*sin(phi)));
                // printf("x+y plus : %lf \n", length);
            } else if (y_wall < 0) {                                        // y hit first on the minus side
                length = fabs((y)/(sin(theta)*sin(phi)));
                // printf("x+y minus : %lf \n", length);
            } else {                                                        // x hit first
                length = fabs((x_wall-x)/(sin(theta)*cos(phi)));
                // printf("x plus : %lf \n", length);
            }


        } else if (x_top < 0) { // Quadrant 3
            x_wall = 0;
            
            // Check if y hit wall first
            y_wall = y + tan(phi) * (x_wall - x);

            if (y_wall > Params.depth) {                                    // y hit first on the plus side
                length = fabs((Params.depth - y)/(sin(theta)*sin(phi)));
                // printf("-x+y plus : %lf \n", length);
            } else if (y_wall < 0) {                                        // y hit first on the minus side
                length = fabs((y)/(sin(theta)*sin(phi)));
                // printf("-x+y minus : %lf \n", length);
            } else {                                                        // x hit first
                length = fabs((x_wall-x)/(sin(theta)*cos(phi)));
                // printf("x minus : %lf \n", length);
            }

        } else if (y_top > Params.depth) { 
            // If we end up here, we know for certain that x is inside the box. Thus, we know the ray hit the y wall
            length = fabs((Params.depth - y)/(sin(theta)*sin(phi)));
            // printf("y plus : %lf \n", length);

        } else {
            // Here we know that y hit the near wall
            length = fabs((y)/(sin(theta)*sin(phi)));
            // printf("y minus : %lf \n", length);

        }
    }

    Store_Data(outputdata, x, y, theta, phi, length);
}


