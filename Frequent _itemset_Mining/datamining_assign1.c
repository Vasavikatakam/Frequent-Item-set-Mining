#include<stdio.h>
#include<string.h>
#include<malloc.h>

#define MINSUPPORT 0.05

struct mapping
{
	char itemname[100];
	int itemnum;
};

struct node
{
	int data;
	struct node* next;
};

struct transacts
{
	struct node *head;
};

struct itemfrequency
{
	int item;
	int freq;
};

struct treenode
{
	int data,freq;

	struct treenode* parent;
	struct treenode* link;
};

struct header
{
	int item;
	int freq;
	struct treenode *pointer;
};

struct frequentitemsets
{
        struct itemfrequency si[100];
        int length;     
};

struct stack
{
	int data;
	int freq;
	struct stack* next;
};

struct output
{
	int freitems[1000];
	int length;
	int freq;
};


const char* getfield(char* line, int num) 		/* traversing the csv file */
{
	const char* tok;
	for (tok = strtok(line, ";");
			tok && *tok;
			tok = strtok(NULL, ";\n"))
	{
		if (!--num)
			return tok;
	}
	return NULL;
}

struct node* createnode(int data)
{
	struct node* temp;
	temp=(struct node *)malloc(sizeof(struct node));
	temp->data=data;
	temp->next=NULL;
	return temp;
};


struct treenode* createtreenode(int data,struct treenode* parent)
{

	struct treenode* temp;
	temp=(struct treenode *)malloc(sizeof(struct treenode));
	temp->data=data;
	temp->freq=1;
	temp->parent=parent;
	temp->link=NULL;
	return temp;

}

struct stack* createstacknode(int data,int freq)
{
	struct stack* temp;
	temp=(struct stack *)malloc(sizeof(struct stack));
	temp->data=data;
	temp->freq=freq;	
	temp->next=NULL;
	return temp;

}

void sorting(struct itemfrequency it_fr[1000],int it_fr_count)
{
	for(int i=0;i<it_fr_count;i++)
	{
		for(int j=0;j<(it_fr_count-i-1);j++)
		{
			if(it_fr[j].freq<it_fr[j+1].freq)
			{
				int temp=it_fr[j].freq;
				it_fr[j].freq=it_fr[j+1].freq;
				it_fr[j+1].freq=temp;

				temp=it_fr[j].item;
				it_fr[j].item=it_fr[j+1].item;
				it_fr[j+1].item=temp;

			}
		}
	}
}


void makeordered(struct transacts tr[1000],int minsupport,struct itemfrequency it_fr[1000],int no_transacts,struct transacts ordered[1000],int  *no_ordered,int *it_fr_count){

	int length,k,i;
	length=no_transacts;
	int var,len;

	*it_fr_count=0;                                              /* imp  */
	for(k=0;k<length;k++)					/* finding frequencies of individual items */
	{	struct node* temp=tr[k].head;

		while(temp!=NULL)
		{

			var=temp->data;
			len=*it_fr_count;
			int f=0,j;
			for(j=0;j<len;j++)
			{
				if(it_fr[j].item==var)
				{
					f=1;
					it_fr[j].freq=it_fr[j].freq+1;
					break;
				}
			}
			if(f==0)
			{	it_fr[*it_fr_count].item=var;
				it_fr[*it_fr_count].freq=1;

				*it_fr_count=*it_fr_count+1;
			}
			temp=temp->next;
		}
	}

	struct itemfrequency it_fr1[1000];              /* storing the items which are having frequency greater than or equals to min support */
	len=*it_fr_count;
	int it_fr_count1=0;
	for(i=0;i<len;i++)
	{
		if(it_fr[i].freq>=minsupport)
		{

			it_fr1[it_fr_count1].item=it_fr[i].item;
			it_fr1[it_fr_count1].freq=it_fr[i].freq;
			it_fr_count1=it_fr_count1+1;

		}
	}

	memcpy(it_fr,it_fr1,sizeof(it_fr1));
	*it_fr_count=it_fr_count1;

	sorting(it_fr,*it_fr_count);			/* sorting in descending order */
	/*printf("item-frequency table\n");
	for(i=0;i<*it_fr_count;i++)
	{
		printf("%d %d\n",it_fr[i].item,it_fr[i].freq);
	}*/


	int len2,m=0,item=0;
	*no_ordered=0;

	for(k=0;k<length;k++)
	{	
		int start=0;
		for(m=0;m<*it_fr_count;m++)
		{	struct node* temp=tr[k].head;

			while(temp!=NULL)
			{
				if(temp->data==it_fr[m].item)
				{
					if(start==0)
					{
						ordered[*no_ordered].head=createnode(temp->data);
					}
					else
					{
						struct node* temp1=ordered[*no_ordered].head;
						while(temp1->next!=NULL)
						{
							temp1=temp1->next;
						}
						temp1->next=createnode(temp->data);
					}
					start=start+1;


					break;
				}
				temp=temp->next;
			}
		}

		if(start!=0)
		{

		*no_ordered=*no_ordered+1;                              /* imp */ /* contains count of ordered items */
		}
	}
	printf("Ordered list\n");
	
	for(i=0;i<*no_ordered;i++)        
	{
		struct node* temp=ordered[i].head;
		while(temp->next!=NULL)
		{
			printf("%d ",temp->data);
			temp=temp->next;	
		}
		printf("%d\n",temp->data);
	}

}

struct treenode* insert(struct treenode* root,struct transacts tree[1000],struct header hdr[1000],int no_transacts,int headerlength)
{	int length;
	length=no_transacts;
	int k;
	for(k=0;k<length;k++)
	{
		
		int len,len1,i;
		
		struct treenode* parent=root;
		struct node* subtran=tree[k].head;
		
		while(subtran!=NULL)
		{

			struct treenode* temp;

			int len3=headerlength;
			for(int i=0;i<len3;i++)
			{
				if(hdr[i].item==subtran->data)
				{
					if(hdr[i].pointer==NULL)                   /* when inserting new item in the FP-tree */
					{
						temp=createtreenode(subtran->data,parent);
						(hdr[i].pointer)=(struct treenode *)malloc(sizeof(struct treenode));
						(hdr[i].pointer)->link=temp;
						
						parent=temp;

						break;
					}
					else
					{
						int f=0;
						struct treenode* temp1=hdr[i].pointer;   /* if pointer to the item alredy exists in the header table */
						while(temp1->link!=NULL)
						{
							if(temp1->link->parent==parent)
							{
								temp1->link->freq=temp1->link->freq+1;
								parent=temp1->link;
								f=1;
								break;
							}
							else
							{
								temp1=temp1->link;
							}
						}
						if(f!=1)
						{
							temp=createtreenode(subtran->data,parent);
							
							parent=temp;

							temp1->link=temp;
						}
					}

					break;
				}	
			}
			subtran=subtran->next;
		}

	}
	return root;
}

/* creating FP-tree */

void createtree(struct transacts tr[1000],int minsupport,struct header hdr[1000],int no_transacts,int *headerlength)
{

	struct treenode* root=NULL;
	struct itemfrequency it_fr[1000];
	struct transacts ordered[1000];

	int no_ordered=0;
	int it_fr_count=0;

	makeordered(tr,minsupport,it_fr,no_transacts,ordered,&no_ordered,&it_fr_count); /* ordering itemsets based on minsupport */
	
	int len=it_fr_count;
	printf("itemfrequency table\n");

	for(int i=0;i<len;i++)
	{
		printf("%d %d\n",it_fr[i].item,it_fr[i].freq);
	}
	
	struct treenode* nul=NULL;

	for(int i=0;i<len;i++)
	{	hdr[i].item=it_fr[i].item;	/* header table to store the item along with the pointer which points to that node in the tree */
		hdr[i].freq=it_fr[i].freq;
		hdr[i].pointer=nul;

	}

	*headerlength=it_fr_count;
	root=insert(root,ordered,hdr,no_ordered,*headerlength);          /* inserting transactions into FP-tree */

	/* traversing */
	printf("GENERATED FP-TREE\n");

	for(int i=len-1;i>=0;i--)
	{
		struct treenode* temp=hdr[i].pointer;
		while(temp->link!=NULL)
		{

			printf("*%d f%d ",temp->link->data,temp->link->freq);
			struct treenode* parent=temp->link->parent;
			while(parent!=NULL)
			{
				printf("%d f%d ",parent->data,parent->freq);
				parent=parent->parent;
			}
			printf("\n");
			temp=temp->link;	

		}
	}
}


void Allsubsets(struct itemfrequency singlepath[1000],struct frequentitemsets fi_1[1000],struct itemfrequency subsets[1000],int index,int size,int *no_subsets,int length_subset)
{	

	for (int i = index; i < size; i++) {
 
	subsets[length_subset].item=singlepath[i].item;
	subsets[length_subset].freq=singlepath[i].freq;
       
        length_subset=length_subset+1;
	for(int j=0;j<length_subset;j++)
	{
	fi_1[*no_subsets].si[j].item=subsets[j].item;
	fi_1[*no_subsets].si[j].freq=subsets[j].freq;
	}
	fi_1[*no_subsets].length=length_subset;
	*no_subsets=*no_subsets+1;		
 
        Allsubsets(singlepath,fi_1,subsets,i+1,size,no_subsets,length_subset);
 
        //backtracking
        length_subset=length_subset-1;
	
    }
 
    return;
}

/* recursively mining all the frequent item sets */

void frequentitems(struct transacts tr[1000],int minsupport,int no_transacts,struct stack **conditions,struct frequentitemsets fi[1000],struct 

frequentitemsets notredundant[1000],int *no_frequentitems,int *no_notredundant)    {
	

	struct treenode* root=NULL;
	/* header table */	
	struct header hdr[1000];   
	int headerlength=0;			
	
	createtree(tr,minsupport,hdr,no_transacts,&headerlength); 	/* creating fp tree */           
	
	int headercount=0;
	struct itemfrequency singlepath[1000];
	int start=0;

	for(int i=(headerlength-1);i>=0;i--)
	{
		struct treenode* temp=hdr[i].pointer;
		if(temp->link->link==NULL)
		{
			singlepath[start].item=temp->link->data;
			singlepath[start].freq=temp->link->freq;
			start=start+1;
			headercount=headercount+1;
			temp=temp->link->parent;
		}
		else
		{
			break;
		}

	}
	if(headercount==headerlength)                             /* checking for single path */
	{

		printf("subsets of\n");
		
		for(int i=0;i<start;i++)
		{
			printf("%d f%d ",singlepath[i].item,singlepath[i].freq);
		}

			struct frequentitemsets fi_1[1000];
			
    			int index = 0;
			int size=start;
			struct itemfrequency subsets[1000]; 
			int length_subset=0;
    			int no_subsets=0;	
			Allsubsets(singlepath,fi_1,subsets,index,size,&no_subsets,length_subset); /*finding all subsets if there is only single path in the fp tree*/
	
		struct stack* s=*conditions;
		int no_elements=0;
		struct itemfrequency itemfreq[1000];
		while(s!=NULL)
		{
			printf("*%d %d ",s->data,s->freq);
			itemfreq[no_elements].item=s->data;
			itemfreq[no_elements].freq=s->freq;	
			no_elements=no_elements+1;
			s=s->next;

		}
		printf("\n");
			

			for(int i=0;i<no_subsets;i++)
			{
			int g=0;	
			int size1=fi_1[i].length;		
			for(int j=size1;j<(size1+no_elements);j++)
			{

			fi_1[i].si[j].item=itemfreq[g].item;
			fi_1[i].si[j].freq=itemfreq[g].freq;
			g=g+1;		
			
			}
			fi_1[i].length=size1+no_elements;
			
			}
			
			for(int i=0;i<no_subsets;i++)
			{
					
			for(int j=0;j<fi_1[i].length;j++)
			{
			

			fi[*no_frequentitems].si[j].item=fi_1[i].si[j].item;			/*storing frequent items */
			fi[*no_frequentitems].si[j].freq=fi_1[i].si[j].freq;
					
			
			}
			fi[*no_frequentitems].length=fi_1[i].length;
			*no_frequentitems=*no_frequentitems+1;
			}

		
		printf("\n");
		printf("Removing redundancy\n");
			
			/* checking for redundant itemsets */
			while(1)
			{
			int dummylength=0;
			struct itemfrequency dummy[1000];
			for(int d=0;d<no_elements;d++)
			{
			
			dummy[dummylength].item=itemfreq[d].item;
			dummy[dummylength].freq=itemfreq[d].freq;	
			dummylength=dummylength+1;			
			
			}
			int pen=0;
			for(int d1=0;d1<*no_notredundant;d1++)
			{pen=0;
			struct itemfrequency dummy1[1000];
			int dummy1length=0;				
			for(int i=0;i<notredundant[d1].length;i++)
			{
			dummy1[dummy1length].item=notredundant[d1].si[i].item;
			dummy1[dummy1length].freq=notredundant[d1].si[i].freq;	
			dummy1length=dummy1length+1;	
			}
			if(dummylength==dummy1length)
			{
				sorting(dummy,dummylength);
				sorting(dummy1,dummy1length);
				int w=0;
				for(int y=0;y<dummylength;y++)
				{
					if(dummy[y].item==dummy1[y].item)
					{	
						w=w+1;
					}						
				}
				if(w==dummylength)
				{	pen=1;
					break;				
				}	
			
			}
										
				
			}
			if(pen==0)
			{
				for(int b=0;b<dummylength;b++)
				{
					notredundant[*no_notredundant].si[b].item=dummy[b].item;
					notredundant[*no_notredundant].si[b].freq=dummy[b].freq;
					fi[*no_frequentitems].si[b].item=dummy[b].item;
					fi[*no_frequentitems].si[b].freq=dummy[b].freq;
				}			
					
				notredundant[*no_notredundant].length=dummylength;
				fi[*no_frequentitems].length=dummylength;
				*no_notredundant=*no_notredundant+1;
				*no_frequentitems=*no_frequentitems+1;
			}			
						
					
			printf("\n");
			no_elements=no_elements-1;
			if(no_elements==0)
			{
			break;
			}
			}
			
			
        }
	else								/* if there is no single path */
	{

		int len=headerlength;
		int i;
		for(i=(len-1);i>=0;i--)
		{
			struct treenode* temp;
			temp=(hdr[i].pointer);
			struct stack *s1;
			if(*conditions==NULL)
			{
				*conditions=createstacknode(temp->link->data,hdr[i].freq);          /* bottom up mining */
			}
			else
			{
				s1=*conditions;
				
				while(s1->next!=NULL)
				{
					s1=s1->next;
				}
				s1->next=createstacknode(temp->link->data,hdr[i].freq);
			}

			                          
			struct transacts contr[1000];                                      /* to store itemsets conditioned on a particular item */
			int no_contr=0;
			while(temp->link!=NULL)					
			{
				int freq=temp->link->freq;
				int z=0;
				for(z=0;z<freq;z++)
				{	int start=0;
					struct treenode* parents;
					parents=temp->link->parent;

					

					while(parents!=NULL)
					{	
						if(start==0)
						{
							contr[no_contr].head=createnode(parents->data);
							start=start+1;
						}
						else
						{
							struct node* temp2=contr[no_contr].head;
							while(temp2->next!=NULL)
							{
								temp2=temp2->next;
							}
							temp2->next=createnode(parents->data);
							start=start+1;
						}
						
						parents=parents->parent;
					}

					if(start!=0)
					{
						no_contr=no_contr+1;
						
					}

				}
				temp=temp->link;
			}

			struct stack* s2;
			s2=*conditions;

			while(s2->next!=NULL)
			{
				s2=s2->next;
			}	

			printf("conditioned on %d\n",s2->data);
			printf("Unordered list:\n"); 

			int len2=no_contr;
			
			for(int j=0;j<len2;j++)
			{
				struct node* tempv=contr[j].head;
				while(tempv->next!=NULL)
				{
					printf("%d ",tempv->data);
					tempv=tempv->next;	
				}
				printf("%d\n",tempv->data);
			}

			frequentitems(contr,minsupport,no_contr,& *conditions,fi,notredundant,&*no_frequentitems,&*no_notredundant); /*recursion */
			
			/*popping the top element from the stack */
			
			s1=*conditions;
			int no_elements=0;

			while(s1!=NULL)
			{
				s1=s1->next;	
				no_elements=no_elements+1;

			}
			if(no_elements==1)
			{
				*conditions=NULL;
			}
			else
			{
				s1=*conditions;
				int co=0;
				while(s1->next!=NULL)
				{
					if(co==(no_elements-2))
					{
						break;
					}
					else
					{
						co=co+1;
						s1=s1->next;
					}
				}
				s1->next=NULL;
			}

		}

	}

}

/* finding frequency for complete itemset based on taking the min frequency */

void generatingfrequencies(struct frequentitemsets fi[1000],struct output out[1000],int no_frequentitems)
{
int len=no_frequentitems;
int i, min;
for(i=0;i<len;i++)
{
min=fi[i].si[0].freq;
for(int j=0;j<fi[i].length;j++)
{
if(min>fi[i].si[j].freq)
{
min=fi[i].si[j].freq;

}
out[i].freitems[j]=fi[i].si[j].item;
}
out[i].length=fi[i].length;
out[i].freq=min;

}
}


int main()
{
	FILE* stream = fopen("/home/vasavi/Downloads/7thsemester/DataMining/groceries_subset.csv", "r");

	char line[10000];
	int count=1;	
	struct mapping map[1000];				/*Mapping each item to a number */
	int no_items=0;	
	struct transacts tr[1000];   				/*For storing all the transactions(all items are now converted to numbers)*/
	int no_transacts=0;

	while (fgets(line, 10000, stream))
	{	
		int start=0;

		char* tmp = strdup(line);			/*  one row of csv file will get stored in tmp */
		int len;
		len=strlen(getfield(tmp,1));			/* reading character by character */			
		char c;
		int j=0,i;
		char str1[1000];
		int count1=0;
		for(i=0;i<len;i++)
		{

			c=getfield(tmp,1)[i];

			if(c!=',')
			{
				str1[j]=c;
				j++;	
			}
			else
			{
				str1[j]='\0';

				int len1=strlen(str1);

				if(len1!=0)
				{	
					int veclen=no_items;	
					int f=0;

					for(int h=0;h<veclen;h++)
					{	
						if(strcmp(map[h].itemname,str1)==0)
						{	if(start==0)
							{
								tr[no_transacts].head=createnode(map[h].itemnum);
							}
							else
							{				
								struct node* temp=tr[no_transacts].head;
								while(temp->next!=NULL)
								{
									temp=temp->next;
								}
								temp->next=createnode(map[h].itemnum);
							}
							start=start+1;

							f=1;
							break;
						}
					}	
					if(f==0)
					{	if(count1!=0)
						{	

							strcpy(map[no_items].itemname,str1);
							map[no_items].itemnum=count;

							no_items=no_items+1;

							if(start==0)
							{
								tr[no_transacts].head=createnode(count);
							}
							else
							{
								struct node* temp=tr[no_transacts].head;
								while(temp->next!=NULL)
								{
									temp=temp->next;
								}
								temp->next=createnode(count);
							}
							start=start+1;

							count=count+1;
						}
					}
				}

				int k;
				for(k=0;k<len1;k++)
				{

					str1[k]='\0';
				}	

				j=0;
				count1=count1+1;
			}	
		}	

		free(tmp);
		no_transacts=no_transacts+1;

	}
	int minsupport=no_transacts*MINSUPPORT;				/* finding min support count */

	struct stack *conditions=NULL;					/* stack for storing the conditioned patterns */	

	struct frequentitemsets notredundant[1000],fi[1000];            /* fi for storing every frequent item set along with their individual frequencies,notredundant for removing the redundancies */

	int no_frequentitems=0;
	int no_notredundant=0;	

	frequentitems(tr,minsupport,no_transacts,&conditions,fi,notredundant,&no_frequentitems,&no_notredundant);
	printf("output along with individual frequencies\n");

        for(int i=0;i<no_frequentitems;i++)
        {
        for(int j=0;j<fi[i].length;j++)
        {
        printf("%d f%d ",fi[i].si[j].item,fi[i].si[j].freq);
        }
       	printf("\n");
                        
	}

	printf("final Output\n");

	struct output out[1000];
	generatingfrequencies(fi,out,no_frequentitems);        /*Generating final frequent itemsets along with their frequencies */

	for(int i=0;i<no_frequentitems;i++)
        {
	printf("itemset:");
	printf("{");
        for(int j=0;j<out[i].length;j++)
        {
        printf("No: %d ",out[i].freitems[j]);
	for(int k=0;k<no_items;k++)
	{
		if(map[k].itemnum==out[i].freitems[j])
		{
		printf("Name: %s ",map[k].itemname);
		break;		
		}	
	}

	if(j!=(out[i].length-1))
		{
			printf(",");		
		}				  
}
	printf("}");
	printf("frequency :%d",out[i].freq);	
       	printf("\n");	
	}

	return 0;
}










