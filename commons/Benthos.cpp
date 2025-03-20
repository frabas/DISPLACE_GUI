// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2025 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "Benthos.h"
#include <helpers.h>

#include <string>
#include <vector>

using namespace std;

Benthos::Benthos(int _id,
                 int _marine_landscape,
                 int nbfuncgrp,
                    const vector<Node *> &_nodes,
                    const vector<double> &_prop_funcgr_biomass_per_node,
                    const vector<double> &_prop_funcgr_number_per_node,
                    const vector<double> &_meanw_funcgr_per_node,
                    const vector<double> &_recovery_rates_per_funcgr,
                    const vector<double> &_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr,
                    const vector<double> &_benthos_number_carrying_capacity_K_per_landscape_per_funcgr,
                    bool is_benthos_in_numbers,
                    bool is_benthos_in_longevity_classes,
                    const vector<double> &_h_betas_per_pop,
                    multimap<int,double> &_longevity_classes_condition_per_node
                    )
{

    cout << "creating benthos for " << _marine_landscape << endl;
    id                                = _id;
    marine_landscape                  = _marine_landscape;
    prop_funcgr_biomass_per_node      = _prop_funcgr_biomass_per_node;
    prop_funcgr_number_per_node       = _prop_funcgr_number_per_node;
    meanw_funcgr_per_node             = _meanw_funcgr_per_node;
    recovery_rates_per_funcgr         = _recovery_rates_per_funcgr;
    benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr=_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr;
    benthos_number_carrying_capacity_K_per_landscape_per_funcgr=_benthos_number_carrying_capacity_K_per_landscape_per_funcgr;
    h_betas_per_pop                   =_h_betas_per_pop;

    dout(cout << "for this landscape "<< marine_landscape <<", assigned nodes are: ");
	vector<Node* > p_spe_nodes;
    for(unsigned int n=0; n<_nodes.size(); n++)
	{
		if(_nodes[n]->get_marine_landscape()== marine_landscape)
		{
			p_spe_nodes.push_back (_nodes[  n  ]);
            dout(cout << _nodes[  n  ]->get_idx_node().toIndex() << " ");

            _nodes[n]->set_benthos_id(id);

  
        }
	}

   
    for (unsigned int i = 0; i < p_spe_nodes.size(); i++)
    {
        p_spe_nodes[i]->init_benthos_tot_number_on_node(nbfuncgrp);
        p_spe_nodes[i]->init_benthos_tot_biomass_on_node(nbfuncgrp);
        p_spe_nodes[i]->init_benthos_tot_meanweight_on_node(nbfuncgrp);
        p_spe_nodes[i]->init_benthos_tot_number_K_on_node(nbfuncgrp);
        p_spe_nodes[i]->init_benthos_tot_biomass_K_on_node(nbfuncgrp);
    }

    if(is_benthos_in_longevity_classes)
    {
       cout << "Benthos longevity approach...for this landscape "<< marine_landscape  << endl;
    }


    dout(cout << endl);
    for(unsigned int i=0; i<p_spe_nodes.size(); i++)
	{
       list_nodes.push_back(p_spe_nodes[i]);

        vector<double> init_longevity_classes_condition_per_node;
        if(is_benthos_in_longevity_classes)
        {
           int idx_node = p_spe_nodes[i]->get_idx_node().toIndex();
           multimap<int,double>::iterator lower_it_lgy = _longevity_classes_condition_per_node.lower_bound(idx_node);
           multimap<int,double>::iterator upper_it_lgy = _longevity_classes_condition_per_node.upper_bound(idx_node);
           for (multimap<int, double>::iterator pos=lower_it_lgy; pos != upper_it_lgy; pos++)
           {
               //outc(cout << "check this: " << pos->second << endl);
               // benthos condition per node
               init_longevity_classes_condition_per_node.push_back(pos->second);
           }

           for(unsigned int funcgr=0; funcgr< nbfuncgrp;funcgr++)
              {
               //outc(cout << "funcgr is: " << funcgr << endl);
               //outc(cout << "init_longevity_classes_condition_per_node.at(funcgr) is: " << init_longevity_classes_condition_per_node.at(funcgr) << endl);
              // put an estimate of number per node for this funcgr as total on node times the proportion of the funcgr on that node
              p_spe_nodes[i]->set_benthos_tot_biomass(funcgr, 1.0 * init_longevity_classes_condition_per_node.at(funcgr));
              p_spe_nodes[i]->set_benthos_tot_meanweight(funcgr, 1.0);
              // add carryingcap K
              p_spe_nodes[i]->set_benthos_tot_number_K(funcgr, 1.0);
              p_spe_nodes[i]->set_benthos_tot_biomass_K(funcgr, 1.0);
              //dout (cout << "this longevity group "<< funcgr << "initial condition on this node " << p_spe_nodes[i]->get_idx_node().toIndex() <<
              //       "this marine landscape " << marine_landscape << " is " << init_longevity_classes_condition_per_node.at(funcgr) << endl);
              }

        } else{
          if(is_benthos_in_numbers)
            {

            for(unsigned int funcgr=0; funcgr< nbfuncgrp;funcgr++)
               {
               // put an estimate of number per node for this funcgr as total on node times the proportion of the funcgr on that node
               p_spe_nodes[i]->set_benthos_tot_number(funcgr, p_spe_nodes[i]->get_init_benthos_number() * prop_funcgr_number_per_node.at(funcgr));
               //...and deduce biomass from N*meanw
               p_spe_nodes[i]->set_benthos_tot_biomass(funcgr, p_spe_nodes[i]->get_init_benthos_number()*meanw_funcgr_per_node.at(funcgr) * prop_funcgr_number_per_node.at(funcgr));
               // add meanweight
               p_spe_nodes[i]->set_benthos_tot_meanweight(funcgr, meanw_funcgr_per_node.at(funcgr));
               // add carryingcap K
               p_spe_nodes[i]->set_benthos_tot_number_K(funcgr, benthos_number_carrying_capacity_K_per_landscape_per_funcgr.at(funcgr));
               p_spe_nodes[i]->set_benthos_tot_biomass_K(funcgr, benthos_number_carrying_capacity_K_per_landscape_per_funcgr.at(funcgr)*meanw_funcgr_per_node.at(funcgr) * prop_funcgr_number_per_node.at(funcgr));
               }
               dout (cout << "prop func. grp. biomass on this node " << p_spe_nodes[i]->get_idx_node().toIndex() <<
                      "this marine landscape " << marine_landscape << " is " << prop_funcgr_biomass_per_node.size() << endl);
            }
           else
           {
            for(unsigned int funcgr=0; funcgr< nbfuncgrp;funcgr++)
               {
                // put an estimate of biomass per node for this funcgr as total on node times the proportion of the funcgr on that node
               p_spe_nodes[i]->set_benthos_tot_biomass(funcgr, p_spe_nodes[i]->get_init_benthos_biomass() * prop_funcgr_biomass_per_node.at(funcgr));
               //...and deduce N from B/meanw
               if(meanw_funcgr_per_node.at(funcgr)!=0) p_spe_nodes[i]->set_benthos_tot_number(funcgr, p_spe_nodes[i]->get_init_benthos_biomass()/meanw_funcgr_per_node.at(funcgr) * prop_funcgr_biomass_per_node.at(funcgr));
               // add meanweight
               p_spe_nodes[i]->set_benthos_tot_meanweight(funcgr, meanw_funcgr_per_node.at(funcgr));
               // add carryingcap K
               p_spe_nodes[i]->set_benthos_tot_biomass_K(funcgr, benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.at(funcgr));
               if(meanw_funcgr_per_node.at(funcgr)!=0) p_spe_nodes[i]->set_benthos_tot_number_K(funcgr, benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr.at(funcgr)/meanw_funcgr_per_node.at(funcgr) * prop_funcgr_biomass_per_node.at(funcgr));
               }

           }
        }
    }

    cout << "creating benthos for " << _marine_landscape <<" ...OK" << endl;

}


Benthos::~Benthos()
{
	//dtor
}

int Benthos::get_id() const
{
    return(id);
}

int Benthos::get_marine_landscape() const
{
	return(marine_landscape);
}


const vector<double> &Benthos::get_h_betas_per_pop() const
{
    return(h_betas_per_pop);
}


const vector<double> &Benthos::get_prop_funcgr_biomass_per_node() const
{
    return(prop_funcgr_biomass_per_node);
}

const vector<double> &Benthos::get_prop_funcgr_number_per_node() const
{
    return(prop_funcgr_number_per_node);
}

const vector<double> &Benthos::get_meanw_funcgr_per_node() const
{
    return(meanw_funcgr_per_node);
}

const vector<double> &Benthos::get_recovery_rates_per_funcgr() const
{
    return(recovery_rates_per_funcgr);
}

const vector<double> &Benthos::get_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr() const
{
    return(benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr);
}

const vector<double> &Benthos::get_benthos_number_carrying_capacity_K_per_landscape_per_funcgr() const
{
    return(benthos_number_carrying_capacity_K_per_landscape_per_funcgr);
}


const vector<Node *> &Benthos::get_list_nodes() const
{
	return(list_nodes);
}




void Benthos::recover_benthos_tot_biomass_per_funcgroup(int is_longevity)
{
    dout(cout  << "the benthos recovering...." << endl);

   vector<Node *> list_nodes_this_landsc= get_list_nodes();
   vector<double> all_benthos_tot_biomass = list_nodes_this_landsc.at(0)->get_benthos_tot_biomass();

   double new_benthos_tot_biomass;
   double benthos_tot_biomass;


   // van Denderen et al. 2019
   double K =1.0;

   if(!is_longevity) for(unsigned int n=0; n<list_nodes_this_landsc.size(); n++)
    {
      // Pitcher et al. 2016
      for(unsigned int funcgr = 0; funcgr < all_benthos_tot_biomass.size(); funcgr++)
       {
          K                       = get_benthos_biomass_carrying_capacity_K_per_landscape_per_funcgr().at(funcgr);
          benthos_tot_biomass     = list_nodes_this_landsc.at(n)->get_benthos_tot_biomass(funcgr);
          new_benthos_tot_biomass =((benthos_tot_biomass*K)/
                                         (benthos_tot_biomass+
                                           (K-benthos_tot_biomass)* exp(-get_recovery_rates_per_funcgr().at(funcgr))))+0.0000001;
          list_nodes_this_landsc.at(n)->set_benthos_tot_biomass(funcgr,  new_benthos_tot_biomass); // update on node
       }
    }


      // Van denderen et al. 2019
   if(is_longevity) for(unsigned int n=0; n<list_nodes_this_landsc.size(); n++)
       {
        for(unsigned int funcgr = 0; funcgr < all_benthos_tot_biomass.size(); funcgr++)
          {
          benthos_tot_biomass     = list_nodes_this_landsc.at(n)->get_benthos_tot_biomass(funcgr);

          double sd=0.39; // default
          double r_error=0;
          r_error= exp( 0 + sd*norm_rand() ) / exp((sd*sd)/2.0);
          double recovery= get_recovery_rates_per_funcgr().at(funcgr) * r_error; // for boostrap on recovery r
          new_benthos_tot_biomass= benthos_tot_biomass + (recovery*benthos_tot_biomass*((K - benthos_tot_biomass)/ K )) +0.00000001;

          list_nodes_this_landsc.at(n)->set_benthos_tot_biomass(funcgr,  new_benthos_tot_biomass); // update on node
         }
      }

}


void Benthos::recover_benthos_tot_number_per_funcgroup()
{
    dout(cout  << "the benthos recovering...." << endl);

   vector<Node *> list_nodes_this_landsc= get_list_nodes();
   vector<double> all_benthos_tot_number = list_nodes_this_landsc.at(0)->get_benthos_tot_number();

   for(unsigned int n=0; n<list_nodes_this_landsc.size(); n++)
   {
      // Pitcher et al. 2016
      for(unsigned int funcgr = 0; funcgr < all_benthos_tot_number.size(); funcgr++)
       {
          double K                       = get_benthos_number_carrying_capacity_K_per_landscape_per_funcgr().at(funcgr);
          double benthos_tot_number     = list_nodes_this_landsc.at(n)->get_benthos_tot_number(funcgr);
          double new_benthos_tot_number =(benthos_tot_number*K)/
                                         (benthos_tot_number+
                                           (K-benthos_tot_number)* exp(-get_recovery_rates_per_funcgr().at(funcgr)));

          list_nodes_this_landsc.at(n)->set_benthos_tot_number(funcgr,  new_benthos_tot_number); // update on node
      }
   }

}
