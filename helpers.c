#include "helpers.h"
#include <math.h>

void calc_values_row(int height, int width, int i, int j, RGBTRIPLE image_copy[height][width], double* sum_blue, double* sum_green, double* sum_red, int* amount_pixels);

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // we need to iterate through each row
    for (int i = 0; i < height; i++)
    {
        // and through each column inside the row
        for (int j = 0; j < width; j++)
        {
            // calc. average and round to the nearest integer
            double sum = image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed;
            int average = round(sum / 3);
            // or just divide by 3.0 instead of 3 to not loose decimal parts
            image[i][j].rgbtBlue = average;
            image[i][j].rgbtGreen = average;
            image[i][j].rgbtRed = average;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // loop through each row
    for (int i = 0; i < height; i++)
    {
        // make empty row for the flipped image
        RGBTRIPLE row_flipped[width];
        // and make variable counting from left to right
        int n = 0;

        // and through each pixel (column) starting from the right
        // we commit the pixel to the left of a flipped row
        for (int j = width -1; j >= 0; j--)
        {
            row_flipped[n] = image[i][j];
            n++;
        }
        // commit the flipped row to the image
        for (int k = 0; k < width; k++)
        {
            image[i][k] = row_flipped[k];
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // create a copy of image to use for the original color values
    RGBTRIPLE image_copy[height][width];
    for (int k = 0; k < height; k++)
    {
        for (int l = 0; l < width; l++)
        {
            image_copy[k][l] = image[k][l];
        }
    }


    // loop through rows
    for (int i = 0; i < height; i++)
    {
        // loop through pixels
        for (int j = 0; j < width; j++)
        {
            double sum_blue = 0;
            double sum_green = 0;
            double sum_red = 0;
            int amount_pixels = 0;

            if (i > 0)
            {
                // include upper row
                calc_values_row(height, width, i-1, j, image_copy, &sum_blue, &sum_green, &sum_red, &amount_pixels);
            }
            if (i < height -1)
            {
                // include lower row
                calc_values_row(height, width, i+1, j, image_copy, &sum_blue, &sum_green, &sum_red, &amount_pixels);
            }
            // include current row
            calc_values_row(height, width, i, j, image_copy, &sum_blue, &sum_green, &sum_red, &amount_pixels);

            // compute averages and commit to picture
            image[i][j].rgbtBlue = round(sum_blue/amount_pixels);
            image[i][j].rgbtGreen = round(sum_green/amount_pixels);
            image[i][j].rgbtRed = round(sum_red/amount_pixels);
        }
    }
    return;
}

void calc_values_row(int height, int width, int i, int j, RGBTRIPLE image_copy[height][width], double* sum_blue, double* sum_green, double* sum_red, int* amount_pixels)
{
    if (j > 0)
    {
        // include left
        *sum_blue = *sum_blue + image_copy[i][j-1].rgbtBlue;
        *sum_green = *sum_green + image_copy[i][j-1].rgbtGreen;
        *sum_red = *sum_red + image_copy[i][j-1].rgbtRed;
        *amount_pixels = *amount_pixels + 1;
    }

    if (j < width -1)
    {
        // include right
        *sum_blue = *sum_blue + image_copy[i][j+1].rgbtBlue;
        *sum_green = *sum_green + image_copy[i][j+1].rgbtGreen;
        *sum_red = *sum_red + image_copy[i][j+1].rgbtRed;
        *amount_pixels = *amount_pixels + 1;
    }

    //include middle pixel
    *sum_blue = *sum_blue + image_copy[i][j].rgbtBlue;
    *sum_green = *sum_green + image_copy[i][j].rgbtGreen;
    *sum_red = *sum_red + image_copy[i][j].rgbtRed;
    *amount_pixels = *amount_pixels + 1;
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    //populate the Gx and Gy matrices
    int Gx[3][3] = {{-1,0,1}, {-2,0,2}, {-1,0,1}};
    int Gy[3][3] = {{-1,-2,-1}, {0,0,0}, {1,2,1}};
    // create a copy of image to use for the original color values
    RGBTRIPLE image_copy[height][width];
    for (int o = 0; o < height; o++)
    {
        for (int p = 0; p < width; p++)
        {
            image_copy[o][p] = image[o][p];
        }
    }

    // loop through rows
    for (int i = 0; i < height; i++)
    {
        // loop through pixels (columns)
        for (int j= 0; j < width; j++)
        {
            // for each pixel now, create a 3x3 box which you use to calculate
            RGBTRIPLE box[3][3];
            for (int k = -1; k < 2; k++)
            {
                for (int l = -1; l < 2; l++)
                {
                    // check if we are out of bounds (of the image)
                    if (i+k < 0 || i+k > height -1 || j+l < 0 || j+l > width -1)
                    {
                        // if we are, treat it as a black pixel
                        box[k+1][l+1].rgbtBlue = 0;
                        box[k+1][l+1].rgbtGreen = 0;
                        box[k+1][l+1].rgbtRed = 0;
                    }
                    else
                    {
                        // else commit the pixel
                        box[k+1][l+1] = image_copy[i+k][j+l];
                    }

                }
            }
            // store gx values
            double gx_red = 0;
            double gx_green = 0;
            double gx_blue = 0;
            // store gy values
            double gy_red = 0;
            double gy_green = 0;
            double gy_blue = 0;

            // now loop through the box to calculate
            for (int n = 0; n < 3; n++)
            {
                for (int m = 0; m < 3; m++)
                {
                    // get Gx values
                    gx_red = gx_red + box[n][m].rgbtRed * Gx[n][m];
                    gx_green = gx_green + box[n][m].rgbtGreen * Gx[n][m];
                    gx_blue = gx_blue + box[n][m].rgbtBlue * Gx[n][m];
                    // get Gy values
                    gy_red = gy_red + box[n][m].rgbtRed * Gy[n][m];
                    gy_green = gy_green + box[n][m].rgbtGreen * Gy[n][m];
                    gy_blue = gy_blue + box[n][m].rgbtBlue * Gy[n][m];
                }
            }
            int red_new = round(sqrt(pow(gx_red, 2) + pow(gy_red, 2)));
            int green_new = round(sqrt(pow(gx_green, 2) + pow(gy_green, 2)));
            int blue_new = round(sqrt(pow(gx_blue, 2) + pow(gy_blue, 2)));

            if (red_new > 255)
            {
                red_new = 255;
            }
            if (green_new > 255)
            {
                green_new = 255;
            }
            if (blue_new > 255)
            {
                blue_new = 255;
            }

            // at the end commit the new pixel to original image
            image[i][j].rgbtBlue = blue_new;
            image[i][j].rgbtGreen = green_new;
            image[i][j].rgbtRed = red_new;
        }
    }
    return;
}