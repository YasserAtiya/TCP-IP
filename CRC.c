/*
 * Yasser Atiya
 * 10/11/2017
 * Project 1: CRC Checksum
 * CNT5505
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

long long sh;

struct data_list
{
   struct data_list *next;
   char data;
};

struct bit_list
{
   int val;
   struct bit_list *next;
   struct bit_list *prev;
};

struct frame
{
   struct data_list *data;
   unsigned short checksum1;
   unsigned short checksum2;
   int size;
   int valid;   
};

struct frame *curframe;
FILE *output;
struct frame* initialize_frame();
int flag_count = 0;
struct data_list *cur, *next;
struct bit_list *bits, *cbits,*pbits;
void append_outputfile();
unsigned short binary_divide(long long divisor, long long dividend);
int set_bit(int oldp, int newp, int val);
long generate_polynomial(unsigned short data);
unsigned short rearrange_bits(unsigned short val);
void print_frame_data();
int c, c2;
int escapes;

int main()
{
   /* open input file */
   FILE *file;
   file  = fopen("output", "r+");
   output = fopen("decoded_output", "w+");
   int FLAG_EXPECTED = 0, DONT_PROCESS = 0, IN_FRAME = 0;
   char cchar;
   int valid=0;
   int B;
   curframe = (struct frame*)initialize_frame();

   if(!file || !output)
   {
      fprintf(stderr, "Error output file not found\n");
      exit(1);
   }

   cur = malloc(sizeof(struct data_list));
   curframe->data = cur;   

   fscanf(file,"%c", &cchar);

   /* iterate through file grabbing characters */
   while((fread(&cchar,1,1,file)))
   {

      /* if in invalid frame */
      if(DONT_PROCESS)
      {
         /* check if frame is ending */ 
         if(cchar == 'b')
         {
            fread(&cchar,1,1,file);
         }
         else if(cchar == 'a')
         {
            /* decrement flag counter and resume reading */
             
            c = fgetc(file);
            if(c != 'a')
               continue;
            DONT_PROCESS=0;
            curframe = (struct frame*)initialize_frame();

            flag_count++;
         }
         continue;
      }



      /* determine if flag or data */
      if(cchar == 'a' && !B)
      {

         /* increment count of flags */
         flag_count++;

         /* if beginning of new frame */
         if(FLAG_EXPECTED)
         {
            FLAG_EXPECTED = 0;

            

            /* perform CRC check on previous frame*/
            curframe->valid =  perform_CRC();        
 
            /* if valid, add previous frame data to ouyput */
            if(curframe->valid)
            {
               append_outputfile();
            }

            /* print frame information */
            print_frame_data();
      
            curframe = (struct frame*)initialize_frame();
   
         }
         /* if end of frame */
         else
         {
            /* wait for next flag */
            FLAG_EXPECTED = 1;
         }
      }
      /* if expected new frame to start, but it did not */ 
      else if(FLAG_EXPECTED)
      {
         /* previous frame invalid */
         /* read until next frame starts */
         DONT_PROCESS = 1;
         printf("skipping data to the next good frame.\n");
      }
      /* if we have data */
      else
      {
         
         if(cchar == 'b' && !B)
         {
            B=1;
            escapes++;
            continue;
         }         

         c = fgetc(file);

         if(c == 'a' && (cchar != 'b' || B))
         {
            curframe->checksum1 = curframe->checksum2 & 255;
            curframe->checksum2 = cchar & 255;
            if(cbits->val != 0)
               cbits->val = cbits->val >> 8; 
            else
               cbits->prev->val = cbits->prev->val >> 8;
            ungetc(c,file);
            curframe->size--;
            B = 0;

            continue;
         }
         else
            curframe->checksum2 = cchar &255;
      
         /* add data to frame */
         next = malloc(sizeof(struct data_list));
         cur->next = next;
         cur->data = cchar;
         cur = next;

         curframe->size++;
       
         /* append data to frame bits */ 
         cbits->val = (cbits->val << 8) | rearrange_bits(cchar);

         /* extend frame bits if needed */
         if(curframe->size%4 == 0) 
         {
            cbits->next = malloc(sizeof(struct bit_list));
            cbits->next->prev = cbits;
            cbits = cbits->next;
            cbits->val = 0;
               
         }

         ungetc(c,file);

      }
         
      B=0;
   }
  
 
   flag_count++;

   if(flag_count%2 == 0)
   {
      
      /* perform CRC check on previous frame*/
      curframe->valid =  perform_CRC();

      /* if valid, add previous frame data to ouyput */
      if(curframe->valid)
      {
         append_outputfile();
      }

      /* print frame information */
      print_frame_data();
   }




   fclose(file);

   return 0;
}

int perform_CRC()
{
   int i = 0;
   long poly, checksum=  0, remainder;
   
   /* combine checksums */
   checksum = ((curframe->checksum1) << 8) | curframe->checksum2;

   cbits = bits;

   sh = cbits->val;

   cbits = cbits->next;

   /* generate polynomial */
   poly = generate_polynomial(sh);

   /* divide by generator */
   remainder = binary_divide(poly,sh);

   /* compare to checksum */
   if(remainder != checksum)
      return 0;
      
   return 1;
   
}

/* perform binary divide */
unsigned short binary_divide(long long divisor, long long dividend)
{
   long long j =0, i = 0, first_out = 0;
   struct data_list *list;
   long long divisor2 = 0, dividend2 = 0, holder = 0;
   int byte_counter = curframe->size-4;
   

   list = curframe->data->next;

   /* iterate through bits */
   for(i=60; i>0; i--)
   {

      /* find leftmost 1 */
      if(divisor > dividend)
      {
 
         if(cbits && cbits->val != 0)
         {

            if(byte_counter >= 4)
            {
               dividend = (dividend << 32);
               i = i+32;
            }
            else
            {
               dividend = (dividend << (8*(byte_counter%4))); 
               i = i+(8*(byte_counter%4));

            }
            
            dividend = dividend | cbits->val;
            cbits = cbits->next;
            byte_counter -= 4;
            continue;
         }

         if(!first_out)
         {
            first_out = 1;
            dividend = dividend << 16;
            
            if(i<47)
               i+=17;
            else
               i=64;
            
            continue;
         }
         
         break;
      }
      else if(dividend-pow(2,i) < 0)
        continue; 

      /* shift divisor to line up with dividend bit */
      divisor2 = divisor << (i-16);

      holder = divisor2;
      dividend2 = dividend;

      /* if not lined up, line up */
      for(j=1; j<64; j++)
      {
         holder= holder>>1;
         dividend2 = dividend2>>1;

         if(holder == 0 && dividend2 != 0)
         {
            divisor2 = divisor2 << 1;
         }
      }

      /* XOR */
         dividend = (dividend ^ divisor2);// & (1<<i)-1;

   }      

   if(dividend > (1<<16))
      dividend = dividend^divisor;

   return dividend;
}

long generate_polynomial(unsigned short data)
{
   long poly = 0;
   unsigned short dat;

   /*set 16th bit */
   poly |= 1<<16;

   /*set 14th bit */
   poly |= 1<<14;

   /*set 13th bit */
   poly |= 1<<13;

   /*set 8th bit */
   poly |= 1<<8;

   /*set 4th bit */
   poly |= 1<<4;

   /*set 1st bit */
   poly |= 1<<1;

   /* end with 1 */
   poly |= 1<<0;
   
   return poly;   
}

unsigned short rearrange_bits(unsigned short val)
{
   int ret = 0, temp = 0;

   /* set old 7th bit as new 7th bit*/
   ret |= set_bit(7,7,val);

   /* set old 3rd bit as new 6th bit */
   ret |= set_bit(3,6,val);

   /* set old 6th bit as new 5th bith */
   ret |= set_bit(6,5,val);

   /* set old 2nd bit as new 4th bit */
   ret |= set_bit(2,4,val);

   /* set old 5th bit as new 3th bit */
   ret |= set_bit(5,3,val);

   /* set old 1st bit as new 2nd bit */ 
   ret |= set_bit(1,2,val);

   /* set old 4th bit as new 1st bit */
   ret |= set_bit(4,1,val);

   /* set old 0th bit as new 0th bit */
   ret |= val & 1;

   return ret;
}

/* return value with bit set at position */
int set_bit(int oldp, int newp, int val)
{
   int ret=0, temp = 0;
   
   temp = val & (1<<(oldp));
   if(oldp>newp)
      ret = temp >> (oldp-newp);
   else 
      ret = temp << (newp-oldp);

   return ret;
}

/* append current frame data to output file */
void append_outputfile()
{
   int i = 0;
   struct data_list* fr;
   fr  = curframe->data;   

   for(i=0; i < curframe->size; i++)
   {
      fputc(fr->data,output);
      fr = fr->next;   
   }   

}

/* initialize frame */
struct frame* initialize_frame()
{
   struct frame *fr;
   fr = malloc(sizeof(struct frame));
   int i = 0;
   struct data_list *clist;

   fr->size = 0;
   fr->valid = 0;
   fr->data = NULL;

   cur = malloc(sizeof(struct data_list));
   fr->data = cur;   

   bits = malloc(sizeof(struct bit_list));
   cbits = bits;
   cbits->val = 0;
   escapes = 0;

   return fr;
}

/* print frame data */
void print_frame_data()
{
   printf("frame %d, size %d: ", ((flag_count/2)-1), (curframe->size+4+escapes));

   if(curframe->valid)
      printf("valid\n");
   else
      printf("invalid\n");

}


/* append other data byte to current divisor */
long append_byte(long target, struct data_list *data)
{
   char next_byte;

   if(!data)
      return -1;

   next_byte = rearrange_bits(data->data);

   target = target << 8;
   target= target | next_byte;

   return target;
}

