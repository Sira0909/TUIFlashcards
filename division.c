#include "stdio.h"
int division(int a,int b){
    int sum=0;
    for(int i = 31; i >0; i--){
        printf("sum: %d   a:%d\n",sum, a>>i);
        sum=sum<<1;
        if (a>>i>b){
            sum++;
            a-=(b<<i);
        }
    }
    return sum;
}

int main(){
    printf("%d",division(12, 3));
}
