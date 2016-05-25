#include "option.h"
#include <stdio.h>

/*
void option_time(struct option_data *opt)
{
   if(!opt->active)
   {
      if(opt->time_out > opt->inactive_time)
      {
      opt->active=true;
      opt->time_out=0;
      }
      else opt->time_out++;
   }
   else
   {
      if(opt->time_in > opt->active_time)
      {
      opt->active=false;
      opt->time_in=0;
      }
      else opt->time_in++;
   }
}
*/

void option_time(struct option_data *opt)
{
    if(opt->active)
    {
        if(opt->time_in > opt->active_time)
        {
            opt->active=false;
            opt->time_in=0;
        }
        else 
            opt->time_in++;
    }
}

bool test_pos_option(struct option_data *opt, struct level_data *currentlevel, int x, int y)
{
   unsigned long address_bmp;
   unsigned char pixelcolor;
   bmp_select(currentlevel->bitmap);
   int ligne;

   for(ligne=0; ligne < opt->option_sprite->h; ligne++)
   {
      address_bmp = bmp_read_line(currentlevel->bitmap, ligne + y);
      for(int colonne=0; colonne < opt->option_sprite->w; colonne++)
      {
      pixelcolor = bmp_read8(address_bmp + colonne + x);
      if ((currentlevel->colormap[pixelcolor].r !=0) ||
          (currentlevel->colormap[pixelcolor].g !=0) ||
          (currentlevel->colormap[pixelcolor].b !=0))
      return(false);
      }
   }
   return(true);
}
/*
void init_option(struct option_data *opt, struct level_data *currentlevel)
{

   if(opt->time_in==0)
   {
   opt->x = rand()%792;
   opt->y = rand()%1200;

      if(!test_pos_option(opt, currentlevel, opt->x, opt->y))
      {
      opt->print_it=false;
      opt->active=false;
      opt->time_out = opt->active_time+1;
      }
      else opt->print_it=true;

   opt->type = rand()%NB_OPT_TYPE;
   }
}
*/

void init_option(struct option_data *opt, struct level_data *currentlevel, struct vaisseau_data *allv, int nombre_vaisseau)
{
    // make sure the option isn't already active
    if(!opt->active)
    {
        // has a ship exploded?
        for(int i=0;i<nombre_vaisseau;i++)
		{            
            // is it at explode_count 100
			if (allv[i].explode_count == opt->explode_appear_time)
            {
                opt->x = allv[i].xpos + (15 - (opt->option_sprite->w / 2));
                opt->y = allv[i].ypos + (15 - (opt->option_sprite->h / 2));

                // make sure the option isnt going to collide with the background
                if(test_pos_option(opt, currentlevel, opt->x, opt->y))
                    opt->active=true;

                opt->type = rand() % NB_OPT_TYPE + 1;
                continue;
            }
        }
    }
}

int take_option(struct option_data *opt, struct vaisseau_data *allv, int nbplayers)
{
    struct vaisseau_data *v;
    for(int i=0; i<nbplayers; i++)
    {
        v = &allv[i];
        if (!v->explode)
            if(abs((v->xpos+16)-(opt->x+6))<=(26+12)/2 && abs((v->ypos+16)-(opt->y+6))<=(28+12)/2)
                return i;
    }
    return -1;
}

void attrib_option(struct option_data *opt, struct vaisseau_data *allv, int test)
{
    struct vaisseau_data *v;
    if(test!=-1)
    {
        v = &allv[test];
        
        //give the ship the option
        v->option_type = opt->type;

        if(opt->type == OPT_MAXFUEL)
            v->fuel = v->max_fuel;

        if(opt->type == OPT_SLOWSHIELD) 
        {
            v->max_shield_force = 214;
            v->speed_shield_force_down = 1;
        }

        opt->active=false;
        opt->time_out=0;
        opt->time_in=0;
        
    }

}

void draw_option(struct option_data *opt, struct level_data *currentlevel)
{
    if(opt->active)
    {
        draw_sprite(currentlevel->level_buffer, opt->option_sprite, opt->x, opt->y);
    }
}

void gestion_option(struct option_data *opt, struct level_data *currentlevel, struct vaisseau_data *allv, struct player_view *views, int nbplayers, int nbviews)
{
    int test;
    option_time(opt);                         // alternance option active ou pas

    init_option(opt, currentlevel, allv, nbplayers);           // init la pos + type de l'option

    if(opt->active)
    {
        
        test = take_option(opt, allv, nbplayers); // -1 = pas prise, sinon num du vaisseau qui prend option
        attrib_option(opt, allv, test);           // gestion type option avec num vaisseau

        draw_option(opt, currentlevel);       // affiche sprite option

    }
}

int init_option_data(struct option_data *opt, char *option_sprite_name, int explode_appear_time, int active_time)
{
    srand(time(NULL));

    opt->active = false;
    opt->time_out = 0;
    opt->time_in = 0;
    opt->type = 0;
    opt->x = 0;
    opt->y = 0;
    opt->active_time = active_time;
    opt->explode_appear_time = explode_appear_time;

    opt->option_sprite = load_bitmap(option_sprite_name,opt->option_sprite_colors);
    if(opt->option_sprite) 
        return(0);
    else 
        return(-1);

}

void unload_option(struct option_data *opt)
{
    if (opt->option_sprite) destroy_bitmap(opt->option_sprite);
}

