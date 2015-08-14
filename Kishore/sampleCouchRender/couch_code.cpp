#include "couch_code.h"


//virtual object model
float couchRot[3] = {0,0,0}, couchScale = 0.6;
float modelPos[3] = {0,0,0}; 
GLuint couchList, couchTex;

void couchCreateModelList() {
 
    //read texture
     IplImage *tex = cvLoadImage("sofa.jpg", CV_LOAD_IMAGE_COLOR);
     //IplImage *tex = cvLoadImage(/*"motherboard.jpg"*/"C:/workspace/ARDemo/sofa6.png", CV_LOAD_IMAGE_COLOR);

    // IplImage *tex = cvLoadImage(/*"motherboard.jpg"*/"C:/workspace/ARDemo/white.jpg", CV_LOAD_IMAGE_COLOR);
    //IplImage *tex = cvLoadImage(/*"motherboard.jpg"*/"C:/workspace/ARDemo/checkerboard_color-1024x768.png", CV_LOAD_IMAGE_COLOR);
	if(tex != NULL) {
		glGenTextures(1, &couchTex);
		glBindTexture(GL_TEXTURE_2D, couchTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1200,1200,/*802, 617,*/ 0, GL_RGB, GL_UNSIGNED_BYTE, tex->imageData);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 1200,1200,/*802, 617,*/ GL_BGR, GL_UNSIGNED_BYTE, tex->imageData );
		// gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 1200,1200,/*802, 617,*/ GL_BGR, GL_UNSIGNED_BYTE, tex->imageData );
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	} else {
		printf("Could not read model texture, exiting...\n");
		exit(0);
	}

    char * dummyPtr;
    char temp[1024];
	float *vertexPos = (float*)malloc(12103*3*sizeof(float));
	if(vertexPos == NULL) {
		printf("Cannot allocate memory for model, exiting...");
		exit(0);
	}

    //float vertexPos[12101][3];
    float color[3][3];
    FILE* file = fopen("sofa.ply","r");
    if(file != NULL) {
        for(int i = 0; i < 18; i++) { //consume header
            dummyPtr = fgets(temp, 1024, file);
        }
		
		float maxX=0, minX=1000, maxY=0, minY=1000, maxZ=0, minZ=1000;

        for(int i = 0; i < /*16135*/12103; i++) { 
            dummyPtr = fgets(temp, 1024, file);
            sscanf(temp, "%f %f %f",&vertexPos[3*i+0], &vertexPos[3*i+1], &vertexPos[3*i+2]);     
			if(maxX < vertexPos[3*i+0])
				maxX = vertexPos[3*i+0];
			if(maxY < vertexPos[3*i+1])
				maxY = vertexPos[3*i+1];
			if(maxZ < vertexPos[3*i+2])
				maxZ = vertexPos[3*i+2];
			if(minX > vertexPos[3*i+0])
				minX = vertexPos[3*i+0];
			if(minY > vertexPos[3*i+1])
				minY = vertexPos[3*i+1];
			if(minZ > vertexPos[3*i+2])
				minZ = vertexPos[3*i+2];

        }

        couchList = glGenLists(1);
        int dummy;
        int texOn = 0;
        int vertex[3];
        float texC[3][2];
        float color[3][3];
        glNewList(couchList, GL_COMPILE);
			glColor3f(1,1,1);
			glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, couchTex);
            for(int i = 0; i < 24104/*23376*/; i++) {
                dummyPtr = fgets(temp, 1024, file);
				                                               /*%d*/
                sscanf(temp, "%d %d %d %d %d %f %f %f %f %f %f       %d %f %f %f %f %f %f %f %f %f", &dummy, &vertex[0], &vertex[1], &vertex[2], &dummy,
                      &texC[0][0],&texC[0][1],&texC[1][0],&texC[1][1],&texC[2][0],&texC[2][1],/*&texOn,*/ &dummy, 
                    &color[0][0],&color[0][1],&color[1][2],&color[1][0],&color[1][1],&color[2][2],&color[2][0],&color[2][1],&color[2][2]);
 
				/*
                if(texOn==0) {
                    glEnable(GL_TEXTURE_2D);
                    glColor3f(1,1,1);
                } else {
                    glDisable(GL_TEXTURE_2D);
                }*/
                glBegin(GL_TRIANGLES);
                    for(int j = 0; j < 3; j++) {
                        if(texOn==0) {
                            glTexCoord2f(texC[j][0], 1-texC[j][1]);
                            glColor3fv(&color[j][0]);
                        } else {
                            glColor3f(0.5, 0.5, 0.5);
                        }
                        glVertex3f(vertexPos[3*vertex[j]+0]/*-53.5985365878*/, vertexPos[3*vertex[j]+1]-500/*-40.6721980338*/,    vertexPos[3*vertex[j]+2]/*-5.0462755209*/);
                    }
                glEnd();
            }
            glEnd();
        glEndList();
        fclose(file);
    } else {
		printf("Could not read model file, exiting...\n");
		exit(0);
	}
	free(vertexPos);
    cvReleaseImage(&tex);
}


void couchDrawModel() {
	
	glPushAttrib(GL_TEXTURE_BIT);

	glPushMatrix();
    glTranslatef(modelPos[0], modelPos[1], modelPos[2]);
    glRotatef(couchRot[2], 0,0,1);
    glRotatef(couchRot[1], 0,1,0);
    glRotatef(couchRot[0], 1,0,0);
    glScalef(couchScale, couchScale, couchScale);
    glCallList(couchList);
    glPopMatrix();   

	glPopAttrib();


}
