#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <set>
#include <iostream>
#include <time.h>
#include <vector>
#include <map>
#include <algorithm>
#include "MersenneTwister.h"
#define PI 3.1415926536

using namespace std;

typedef set<int, std::less<int> > halmaz;	
typedef std::map<int, int> intmap;
typedef std::map<int, long long> longmap;

int meret, l, ido;
intmap nodes;//,e,e2;
long long s0,s1,s2,s3,e1,e2,e3,e4;
longmap fok,foks,foke,fokes;
halmaz mut, in, out, a1,a2,b1,b2,c1,c2,d1,d2;

//inline
int nex(int w, int step)
	{//x iranyban step-nyivel arrebb levo pont koordinatajat adja vissza
	w=(w-step)%meret;
	/*if (w<0)
		{w+=meret;
		}	
	return(w);*/
	return((w<0) ? (w+meret) : w);
	};


/*int squareentropy(int x, int y, int l)
	{//cout<< x << " "<< y<<"\11";
	int d;
	halmaz mut, in, out;//, a1,a2,b1,b2,c1,c2,d1,d2;
	for(int i=0;i<l;i++)
		{d=nex(x,-i)+meret*y; if (nodes.find(d)!=nodes.end()) {in.insert(nodes[d]);}
		d=nex(x,-l+1)+meret*nex(y,-i); if (nodes.find(d)!=nodes.end()) {in.insert(nodes[d]);}
		d=nex(x,-i)+meret*nex(y,-l+1); if (nodes.find(d)!=nodes.end()) {in.insert(nodes[d]);}
		d=x+meret*nex(y,-i); if (nodes.find(d)!=nodes.end()) {in.insert(nodes[d]);}
		
		d=nex(x,-i)+meret*nex(y,1); if (nodes.find(d)!=nodes.end()) {out.insert(nodes[d]);}
		d=nex(x,-l)+meret*nex(y,-i); if (nodes.find(d)!=nodes.end()) {out.insert(nodes[d]);}
		d=nex(x,-i)+meret*nex(y,-l); if (nodes.find(d)!=nodes.end()) {out.insert(nodes[d]);}
		d=nex(x,1)+meret*nex(y,-i); if (nodes.find(d)!=nodes.end()) {out.insert(nodes[d]);}
		}
	set_intersection(in.begin(),in.end(),out.begin(),out.end(), std::insert_iterator< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	s3+=mut.size();
	return(mut.size());
	}*/

void newentropy(int x, int y)
	{//cout<< x << " "<< y<<"\11";
	//int l=div(meret,2).quot;
	//cout << "l:"<<l<<"\n";
	int d;
	//szukseg van 16+2 halmazra
	//halmaz mut, in, out, a1,a2,b1,b2,c1,c2,d1,d2;
	mut.clear();in.clear();out.clear();a1.clear();a2.clear();b1.clear();b2.clear();c1.clear();c2.clear();d1.clear();d2.clear();
	/*
	 --- ---
	| 1 | 1 |
	|4a2|4b2|
	| 3 | 3 |
	 ---+---
	| 1 | 1 |
	|4d2|4c2|
	| 3 | 3 |
	 --- ---
	*/
	//int f,g;
	for(int i=0;i<l;i++)
		{d=nex(x,-i)+meret*y; if (nodes.find(d)!=nodes.end()) {a1.insert(nodes[d]);}
		//cout <<d <<" ";
		d=nex(x,-l+1)+meret*nex(y,-i); if (nodes.find(d)!=nodes.end()) {a2.insert(nodes[d]);}
		//cout <<d <<" ";
		d=nex(x,-i)+meret*nex(y,-l+1); if (nodes.find(d)!=nodes.end()) {a1.insert(nodes[d]);}
		//cout <<d <<" ";
		d=x+meret*nex(y,-i); if (nodes.find(d)!=nodes.end()) {a2.insert(nodes[d]);}
		//cout <<d <<"\n";
		
		d=nex(x,-l-i)+meret*y; if (nodes.find(d)!=nodes.end()) {b1.insert(nodes[d]);}
		d=nex(x,-2*l+1)+meret*nex(y,-i); if (nodes.find(d)!=nodes.end()) {b2.insert(nodes[d]);}
		d=nex(x,-l-i)+meret*nex(y,-l+1); if (nodes.find(d)!=nodes.end()) {b1.insert(nodes[d]);}
		d=nex(x,-l)+meret*nex(y,-i); if (nodes.find(d)!=nodes.end()) {b2.insert(nodes[d]);}
		
		d=nex(x,-l-i)+meret*nex(y,-l); if (nodes.find(d)!=nodes.end()) {c1.insert(nodes[d]);}
		d=nex(x,-2*l+1)+meret*nex(y,-l-i); if (nodes.find(d)!=nodes.end()) {c2.insert(nodes[d]);}
		d=nex(x,-l-i)+meret*nex(y,-2*l+1); if (nodes.find(d)!=nodes.end()) {c1.insert(nodes[d]);}
		d=nex(x,-l)+meret*nex(y,-l-i); if (nodes.find(d)!=nodes.end()) {c2.insert(nodes[d]);}
		
		d=nex(x,-i)+meret*nex(y,-l); if (nodes.find(d)!=nodes.end()) {d1.insert(nodes[d]);}
		d=nex(x,-l+1)+meret*nex(y,-i-l); if (nodes.find(d)!=nodes.end()) {d2.insert(nodes[d]);}
		d=nex(x,-i)+meret*nex(y,-2*l+1); if (nodes.find(d)!=nodes.end()) {d1.insert(nodes[d]);}
		d=x+meret*nex(y,-l-i); if (nodes.find(d)!=nodes.end()) {d2.insert(nodes[d]);}
		}
	//negyzetek:	
	//a
	//in.clear();out.clear();mut.clear();
	in.insert(a1.begin(),a1.end());in.insert(a2.begin(),a2.end());
	out.insert(d1.begin(),d1.end());out.insert(b2.begin(),b2.end());
	e1+=in.size();
	//e1+=out.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(), std::insert_iterator< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	s0+=mut.size();
	//b
	in.clear();out.clear();mut.clear();
	in.insert(b1.begin(),b1.end());in.insert(b2.begin(),b2.end());
	out.insert(c1.begin(),c1.end());out.insert(a2.begin(),a2.end());
	e1+=in.size();
	//e2+=out.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(),std::insert_iterator< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	s0+=mut.size();
	//c
	in.clear();out.clear();mut.clear();
	in.insert(c1.begin(),c1.end());in.insert(c2.begin(),c2.end());
	out.insert(b1.begin(),b1.end());out.insert(d2.begin(),d2.end());
	e1+=in.size();
	//e2+=out.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(),std::insert_iterator< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	s0+=mut.size();
	//d
	in.clear();out.clear();mut.clear();
	in.insert(d1.begin(),d1.end());in.insert(d2.begin(),d2.end());
	out.insert(a1.begin(),a1.end());out.insert(c2.begin(),c2.end());
	e1+=in.size();
	//e2+=out.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(),std::insert_iterator< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	s0+=mut.size();
	//csikok:
	//a+b
	in.clear();out.clear();mut.clear();
	in.insert(a1.begin(),a1.end());in.insert(b1.begin(),b1.end());
	out.insert(d1.begin(),d1.end());out.insert(c1.begin(),c1.end());
	e2+=in.size();
	e2+=out.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(),std::insert_iterator< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	s1+=2*mut.size();
	//a+d
	in.clear();out.clear();mut.clear();
	in.insert(a2.begin(),a2.end());in.insert(d2.begin(),d2.end());
	out.insert(b2.begin(),b2.end());out.insert(c2.begin(),c2.end());
	e2+=in.size();
	e2+=out.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(),std::insert_iterator< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	s1+=2*mut.size();
	
	//s2+=crossentropy(x,y,l);
	//return(0);
	}

/*
int lef(int a)
	{while((a % 2)==0)
		{a/=2;
		}
	return(a);
	}
*/


void paraentropy(int x, int y, int db)
	{//alfa fokban van megadva!
	//cout<< x << " "<< y<<"\11";
	//int l=meret/2;
	//cout << "l:"<<l<<"\n"
	int d;
	double a=1.0;
	for(int n=1;n<=db;n++)
		{//a=(double (n));
	//cout << a<< "\n";
	//szukseg van 8+3 halmazra
	//halmaz mut, in, out, a1,a2,b1,b2,c1,c2,d1,d2;
	//a1.clear();a2.clear();b1.clear();b2.clear();c1.clear();c2.clear();d1.clear();d2.clear();
	mut.clear();in.clear();out.clear();a1.clear();a2.clear();b1.clear();b2.clear();c1.clear();c2.clear();d1.clear();d2.clear();
	//int x2=(int (a*(double (l))));
	//int y2=(int ((double (l-1))/a));
	//int f,g;
	//alja+teteje
	for(int i=0;i<l;i++)
		{d=nex(x,-i)+meret*y; if (nodes.find(d)!=nodes.end()) {a1.insert(nodes[d]);}
		//a1.insert(nodes[nex(x,-i)+meret*y]);
		//lna1 << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
		d=nex(x,n*(l-1)-i)+meret*nex(y,-l+1); if (nodes.find(d)!=nodes.end()) {a1.insert(nodes[d]);}
		//a1.insert(nodes[nex(x,n*(l-1)-i)+meret*nex(y,-l+1)]);
		//lna1 << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
		
		d=nex(x,-l-i)+meret*y; if (nodes.find(d)!=nodes.end()) {b1.insert(nodes[d]);}
		//b1.insert(nodes[nex(x,-l-i)+meret*y]);
		//lnb1 << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
		d=nex(x,n*(l-1)-l-i)+meret*nex(y,-l+1); if (nodes.find(d)!=nodes.end()) {b1.insert(nodes[d]);}
		//b1.insert(nodes[nex(x,n*(l-1)-l-i)+meret*nex(y,-l+1)]);
		//lnb1 << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
		
		d=nex(x,-l-i+n*l)+meret*nex(y,-l); if (nodes.find(d)!=nodes.end()) {c1.insert(nodes[d]);}
		//c1.insert(nodes[nex(x,-l-i+n*l)+meret*nex(y,-l)]);
		//lnc1 << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
		d=nex(x,-l-i+n*(2*l-1))+meret*nex(y,-2*l+1); if (nodes.find(d)!=nodes.end()) {c1.insert(nodes[d]);}
		//c1.insert(nodes[nex(x,-l-i+n*(2*l-1))+meret*nex(y,-2*l+1)]);
		//lnc1 << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
		
		d=nex(x,-i+n*l)+meret*nex(y,-l); if (nodes.find(d)!=nodes.end()) {d1.insert(nodes[d]);}
		//d1.insert(nodes[nex(x,-i+n*l)+meret*nex(y,-l)]);
		//lnd1 << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
		d=nex(x,-i+n*(2*l-1))+meret*nex(y,-2*l+1); if (nodes.find(d)!=nodes.end()) {d1.insert(nodes[d]);}
		//d1.insert(nodes[nex(x,-i+n*(2*l-1))+meret*nex(y,-2*l+1)]);
		//lnd1 << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
		}
		
	//oldalak:
	for(int i=0;i<n*l;i++)
			{//lankas, x szerint megyunk
			//d=nex(x,-l+1+(int (a*(double (i)))))+meret*nex(y,-i); if (nodes.find(d)!=nodes.end()) {a2.insert(nodes[d]);}
			d=nex(x,-l+1+i)+meret*nex(y,(int ((double (-i))/a))); if (nodes.find(d)!=nodes.end()) {a2.insert(nodes[d]);}
			//lna << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
			d=nex(x,i-n+1)+meret*nex(y,(int ((double (-i))/a))); if (nodes.find(d)!=nodes.end()) {a2.insert(nodes[d]);}
			//lna << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
			
			d=nex(x,-2*l+i+1)+meret*nex(y,(int ((double (-i))/a))); if (nodes.find(d)!=nodes.end()) {b2.insert(nodes[d]);}
			//lnb << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
			d=nex(x,-l+i-n+1)+meret*nex(y,(int ((double (-i))/a))); if (nodes.find(d)!=nodes.end()) {b2.insert(nodes[d]);}
			//lnb << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
			
			d=nex(x,-2*l+i+n*l+1)+meret*nex(y,-l+(int ((double (-i))/a))); if (nodes.find(d)!=nodes.end()) {c2.insert(nodes[d]);}
			//lnc << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
			d=nex(x,-l+i+n*l-n+1)+meret*nex(y,-l+(int ((double (-i))/a))); if (nodes.find(d)!=nodes.end()) {c2.insert(nodes[d]);}
			//lnc << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
		
			d=nex(x,-l+i+n*l+1)+meret*nex(y,-l+(int ((double (-i))/a))); if (nodes.find(d)!=nodes.end()) {d2.insert(nodes[d]);}
			//lnd << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
			d=nex(x,n*l+i-n+1)+meret*nex(y,-l+(int ((double (-i))/a))); if (nodes.find(d)!=nodes.end()) {d2.insert(nodes[d]);}
			//lnd << div(d,meret).rem<< "\11" << div(d,meret).quot << "\n";
			}
	//negyzetek:	
	//a
	//in.clear();out.clear();mut.clear();
	in.insert(a1.begin(),a1.end());in.insert(a2.begin(),a2.end());
	out.insert(d1.begin(),d1.end());out.insert(b2.begin(),b2.end());
	foke[n]+=in.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(), std::inserter< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	fok[n]+=mut.size();
	//s0+=mut.size();
	//b
	in.clear();out.clear();mut.clear();
	in.insert(b1.begin(),b1.end());in.insert(b2.begin(),b2.end());
	out.insert(c1.begin(),c1.end());out.insert(a2.begin(),a2.end());
	foke[n]+=in.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(),std::inserter< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	//s0+=mut.size();
	fok[n]+=mut.size();
	//c
	in.clear();out.clear();mut.clear();
	in.insert(c1.begin(),c1.end());in.insert(c2.begin(),c2.end());
	out.insert(b1.begin(),b1.end());out.insert(d2.begin(),d2.end());
	foke[n]+=in.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(),std::inserter< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	fok[n]+=mut.size();
	//s0+=mut.size();
	//d
	in.clear();out.clear();mut.clear();
	in.insert(d1.begin(),d1.end());in.insert(d2.begin(),d2.end());
	out.insert(a1.begin(),a1.end());out.insert(c2.begin(),c2.end());
	foke[n]+=in.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(),std::inserter< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	fok[n]+=mut.size();
	//s0+=mut.size();
	//csikok:
	//a+b
	in.clear();out.clear();mut.clear();
	in.insert(a1.begin(),a1.end());in.insert(b1.begin(),b1.end());
	out.insert(d1.begin(),d1.end());out.insert(c1.begin(),c1.end());
	fokes[n]+=in.size();
	fokes[n]+=out.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(),std::inserter< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	//s1+=2*mut.size();
	foks[n]+=2*mut.size();
	//a+d
	in.clear();out.clear();mut.clear();
	in.insert(a2.begin(),a2.end());in.insert(d2.begin(),d2.end());
	out.insert(b2.begin(),b2.end());out.insert(c2.begin(),c2.end());
	fokes[n]+=in.size();
	fokes[n]+=out.size();
	set_intersection(in.begin(),in.end(),out.begin(),out.end(),std::inserter< halmaz >( mut, mut.begin() ) );
	//cout <<mut.size()<<"\11";
	//s1+=2*mut.size();
	foks[n]+=2*mut.size();
	//in.clear();out.clear();mut.clear();
	//s2+=crossentropy(x,y,l);
	a+=1.0;	
		}
	//return(0);
	}

#define pi 3.1415926535
int main(int argc, char *argv[])
	{ido=(int) time(NULL);
	//int klaszter,duc;
	//halmaz::iterator cit, sit;
	//num.clear();
	//map<int, long long >::iterator id;
	int nov=6;
//	int lid;
	if(argc<nov)
		{//Hibas inditas eseten hibauzenet
		cout << "Needed inputs: 1:oszlopok szama,2: binwidth, 3:file \n";
	  	char wait; 
		cin >> wait;
	  	return 1;
     	};
	int rid=atoi(argv[1]);	
	meret = atoi(argv[2]);
	int real= atoi(argv[3]);
	int trial= atoi(argv[4]);
	char *outfile=argv[7];
	
	//hatul kicsi a rendszer, felesleges annyi poziciot szamolni:
	if (meret*meret<trial)
		{trial=meret*meret;
		}
	l=meret/2;	
	//double delta= atof(argv[5]);
	char *sn;
	sn=argv[5];
		FILE *ls;
		if(!(ls=fopen(sn,"r")))
    {
      fprintf(stderr,"Sikertelen bemeneti file megnyitasi kiserlet!\n");
      exit(-1);
    } 
	int fend=0;
	char random_seed; 
	if(argc>nov)
		{fend = atoi(argv[nov]);
		}
	else
		{FILE *random_file = fopen("/dev/urandom","r");
	if (!random_file)
		{cout << "Zaj fajl megnyitasa sikertelen!\n";
		exit(-1);
		};
	random_seed = getc(random_file);
	fend=random_seed;
		}
//MTRand mtrand(fend*ido);
	MTRand mtrand(fend);
	
	char fs[255];	
	sprintf(fs,"%s/gap/square/bp_%d_%d_%d_%d.cor",outfile,rid,meret,real,fend);
	ofstream logsta(fs, ios::out);
	if (!logsta)
		{cout << "Kimeneti fajl megnyitasa sikertelen!\n";
		exit(-1);
		};

		s0=0;s1=0;s2=0;s3=0;
		e1=0;e2=0;e3=0;e4=0;
		int t, fo;
		nodes.clear();
		//realizacio eleje:		
		//for(int realizacio=1;realizacio<=real;realizacio++)
		int da=20;
		for(fo=1;fo<=da;fo+=1)
			{fok[fo]=0;
			foks[fo]=0;
			foke[fo]=0;
			fokes[fo]=0;
			}	
		int realizacio=0;
		char nev[255];
		int i,s;		
		while (!feof(ls))
		{fscanf(ls,"%s\n",nev);
		realizacio++;
		//cout << "R"<<realizacio<<"\11"<< nev;
		ifstream lsm ( nev , ifstream::in );
		for(i=0; i<meret*meret; i++)
			{//fscanf(lsm,"%Ld",&s);
			lsm >> s;
			nodes[i]=s;
			}
		/*nodes.clear();
		//be kell olvasni a fajlt:
		int i=0,s=0;
		while (!feof(ls))
			{//voltez=0;
			fscanf(ls,"%d",&s);
			nodes[i]=s;
			//cout << nodes[i] <<"\11";
			i++;
			//cluster.insert(lid);
			}
		*/
	//johet az entropiaszamolas
	for(t=0;t<trial;t++)
		{//generalunk egy random poziciot es kiszamoljuk az entropiakat:
		newentropy(mtrand.randInt(meret-1),mtrand.randInt(meret-1));
		
		//for(fo=1;fo<=da;fo++)
			{paraentropy(mtrand.randInt(meret-1),mtrand.randInt(meret-1),da);
			}
		}
	/*trial=meret*meret;	
	for(int i=0;i<meret;i++)
		{//generalunk egy random poziciot es kiszamoljuk az entropiakat:
		for(int j=0;j<meret;j++)
		{
		newentropy(i,j);
		}
		}*/
	
		//cout << s0 << "\11" << s1 << "\11" << s1-s0 << "\n";
		//realizacio vege
		}	
		
		//cout << 90 << "\11"<<(double (s0))/trial/real/4<< "\11" << (double (s1))/trial/real/4<< "\n";
		//logsta << 90 <<"\11"<<(double (s0))/trial/real/4<< "\n";
		
		//logsta << 90 <<"\11" <<(double (s1-s0))/trial/real/4 << "\n";
		logsta << 0  << "\11"<< 90  << "\11"<<((double) s0)/trial/realizacio/4<< "\11" << ((double) (s1-s0))/trial/realizacio/4<<  "\11" <<((double) e1)/trial/realizacio/4<<  "\11" <<((double) (e2-e1))/trial/realizacio/4<<"\n";
		
		for(fo=1;fo<=da;fo++)
			{//generalunk egy random poziciot es kiszamoljuk az entropiakat:
			logsta  << fo<<"\11" << atan(1/(double (fo)))*180/PI  <<"\11" <<(double (fok[fo]))/trial/realizacio/4 <<"\11" <<(double (foks[fo]-fok[fo]))/trial/realizacio/4 << "\11" <<(double (foke[fo]))/trial/realizacio/4 <<"\11" <<(double (fokes[fo]-foke[fo]))/trial/realizacio/4 <<"\n";
			//cout << fo <<"\11" <<(double (foks[fo]-fok[fo]))/trial/real/4 << "\n";
			}
		cout << (int) time(NULL)-ido;
		logsta.close();
		//lnu.close();
		//logd.close();
		return 0;
		}
