


fmt_str='bunny_in_Inches_no_standoff_%i.snr.exported';
Diag= 6.3336492091595584; %Bunny


MyOut=[];
close all;

for i=1:12,
    
    Filename=sprintf(fmt_str,(i-1)*30);
    
    A = importdata(Filename);
    
    SignedDist = A(:,4);
    
    Dist = abs(SignedDist);
    
    MaxDist = max(Dist);
   
    AvgDist = mean(Dist);

    MyOut(i,:)= [MaxDist, AvgDist, MaxDist/Diag, AvgDist/Diag];
    
    sprintf('Scan % i : Max Dist = %0.5g  , Mean Dist = %0.5g , Relative Max Dist = %0.5g , Relative Mean Dist = %0.5g \n',i,MyOut(i,1), MyOut(i,2), MyOut(i,3), MyOut(i,4))
    
    figure;
    
    Hist(Dist,100);
    
end

MyOut