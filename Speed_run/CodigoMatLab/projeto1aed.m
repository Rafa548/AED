load run1
load run2
load run3
load run4
load run5
load run6

% run time
r1_time = run1(:,4);
r1_pos = run1(:,1);
lt = log10(r1_time);
X=[r1_pos,0*r1_pos+1];
w = pinv(X)*r1_time;
w(1)*800+w(2)
A=X*w;
subplot("Position",[0.10 0.55 0.36 0.30])
plot(r1_pos,lt,"r",r1_pos,A,"g")
xlabel("Position ")
ylabel("Time (s)")
title("Brute Force")
legend("Position/Time (log10)","Position/Time (linear)")

r2_time = run2(:,4);
r2_pos = run2(:,1);
lt = log10(r2_time);
X=[r2_pos,0*r2_pos+1];
w = pinv(X)*r2_time;
B=X*w;
w(1)*800+w(2)
subplot("Position",[0.53 0.55 0.36 0.30])
plot(r2_pos,lt,"r",r2_pos,B,"g")
xlabel("Position")
ylabel("Time (s)")
title("Branch and Bound (incomplete)")
legend("Position/Time (log10)","Position/Time (linear)")

r3_time = run3(:,4);
r3_pos = run3(:,1);
lt = log10(r3_time);
X=[r3_pos,0*r3_pos+1];
w = pinv(X)*lt;
C=X*w;
subplot("Position",[0.10 0.07 0.36 0.30])
plot(r3_pos,lt,"r",r3_pos,C,"g")
xlabel("Position")
ylabel("Time (s)")
title("Branch and Bound (complete -> brake first)")
legend("Position/Time (log10)","Position/Time (linear)")


r4_time = run4(:,4);
r4_pos = run4(:,1);
lt = log10(r4_time);
X=[r4_pos,0*r4_pos+1];
w = pinv(X)*lt;
D=X*w;
subplot("Position",[0.53 0.07 0.36 0.30])
plot(r4_pos,lt,"r",r4_pos,D,"g")
xlabel("Position")
ylabel("Time (s)")
title("Branch and Bound (complete -> accelerate first (v1))")
legend("Position/Time (log10)","Position/Time (linear)")


r5_time = run5(:,4);
r5_pos = run5(:,1);
lt = log10(r5_time);
X=[r5_pos,0*r5_pos+1];
w = pinv(X)*lt;
E=X*w;
figure (2)
subplot("Position",[0.10 0.55 0.36 0.30])
plot(r5_pos,lt,"r",r5_pos,E,"g")
xlabel("Position")
ylabel("Time (s)")
title("Branch and Bound(complete -> accelerate first (v2))")
legend("Position/Time (log10)","Position/Time (linear)")


r6_time = run6(:,4);
r6_pos = run6(:,1);
lt = log10(r6_time);
X=[r6_pos,0*r6_pos+1];
w = pinv(X)*lt;
F=X*w;
subplot("Position",[0.53 0.55 0.36 0.30])
plot(r6_pos,lt,"r",r6_pos,F,"g")
xlabel("Position")
ylabel("Time (s)")
title("Branch and Bound(complete -> accelerate first (v3))")

legend("Position/Time (log10)","Position/Time (linear)")

subplot("Position",[0.10 0.07 0.36 0.30])
plot(r1_pos,A,"blue",r2_pos,B,"red",r3_pos,C,"green",r4_pos,D,"cyan",r5_pos,E,"magenta",r6_pos,F,"black")
title("Position/Time (linear)")
xlabel("Position")
ylabel("Time (s)")
legend("BF","BaB(Inc.)","BaB (Complete)","BaB (v1)","BaB (v2)","BaB (v3)")
%%
% move number
load run1    
load run2
load run3
load run4
load run5
load run6

r1_nmoves = run1(:,2);
r1_pos = run1(:,1);
X=[r1_pos,0*r1_pos+1];
w = pinv(X)*r1_nmoves;
w(1)*800+w(2)

figure(1)
subplot("Position",[0.10 0.55 0.36 0.30])
plot(r1_pos,r1_nmoves)
xlabel("Position")
ylabel("Nº Moves")
title("Brute Force")

r2_nmoves = run2(:,2);
r2_pos = run2(:,1);
X=[r2_pos,0*r2_pos+1];
w = pinv(X)*r2_nmoves;
w(1)*800+w(2)

subplot("Position",[0.53 0.55 0.36 0.30])
plot(r2_pos,r2_nmoves)
xlabel("Position")
ylabel("Nº Moves")
title("Branch and Bound (incomplete)")
r3_nmoves = run3(:,2);
r3_pos = run3(:,1);

subplot("Position",[0.10 0.07 0.36 0.30])
plot(r3_pos,r3_nmoves)
xlabel("Position")
ylabel("Nº Moves")
title("Branch and Bound (complete -> brake first)")


r4_nmoves=run4(:,2);
r4_pos=run4(:,1);


subplot("Position",[0.53 0.07 0.36 0.30])
plot(r4_pos,r4_nmoves)
xlabel("Position")
ylabel("Nº Moves")
title("Branch and Bound (complete -> accelerate first (v1))")

r5_nmoves=run5(:,2);
r5_pos=run5(:,1);

figure (2)
subplot("Position",[0.10 0.55 0.36 0.30])
plot(r5_pos,r5_nmoves)
xlabel("Position")
ylabel("Nº Moves")
title("Branch and Bound (complete -> accelerate first (v2))")



r6_nmoves=run6(:,2);
r6_pos=run6(:,1);

subplot("Position",[0.53 0.55 0.36 0.30])
plot(r6_pos,r6_nmoves)
xlabel("Position")
ylabel("Nº Moves")
title("Branch and Bound (complete -> accelerate first (v3))")


%%
% effort
load run1    
load run2
load run3
load run4
load run5
load run6

r1_effort = run1(:,3);
r1_pos = run1(:,1);
lt = log10(r1_effort);
X=[r1_pos,0*r1_pos+1];
w = pinv(X)*r1_effort;
w(1)*800+w(2)


figure(1)
subplot("Position",[0.10 0.55 0.36 0.30])
plot(r1_pos,lt)
xlabel("Position")
ylabel("Effort")
title("Brute Force")

r2_effort = run2(:,3);
r2_pos = run2(:,1);
lt=log10(r2_effort);
X=[r2_pos,0*r2_pos+1];
w = pinv(X)*r2_effort;
w(1)*800+w(2)

subplot("Position",[0.53 0.55 0.36 0.30])
plot(r2_pos,lt)
xlabel("Position")
ylabel("Effort")
title("Branch and Bound (incomplete)")

r3_effort = run3(:,3);
r3_pos = run3(:,1);

subplot("Position",[0.10 0.07 0.36 0.30])
plot(r3_pos,r3_effort)
xlabel("Position")
ylabel("Effort")
title("Branch and Bound (complete -> brake first)")

r4_effort = run4(:,3);
r4_pos = run4(:,1);
subplot("Position",[0.53 0.07 0.36 0.30])
plot(r4_pos,r4_effort)
xlabel("Position")
ylabel("Effort")
title("Branch and Bound (complete -> accelerate first (v1))")


r5_effort = run5(:,3);
r5_pos = run5(:,1);
figure (2)
subplot("Position",[0.10 0.55 0.36 0.30])
plot(r5_pos,r5_effort)
xlabel("Position")
ylabel("Effort")
title("Branch and Bound (complete -> accelerate first (v2))")



r6_effort = run6(:,3);
r6_pos = run6(:,1);
subplot("Position",[0.53 0.55 0.36 0.30])
plot(r6_pos,r6_effort)
xlabel("Position")
ylabel("Effort")
title("Branch and Bound (complete -> accelerate first (v3))")

%%
% maxspeed
load maxspeed1

maxspeed=maxspeed1(:,2);
position=maxspeed1(:,1);
accumulatedspeed=maxspeed1(:,3);
bestnmoves=maxspeed1(:,4);

figure(1)
subplot("Position",[0.10 0.55 0.36 0.30])
plot(position,maxspeed)
xlabel("Position")
ylabel("Max Speed")
title("Position / Max Speed")

subplot("Position",[0.53 0.55 0.36 0.30])

plot(position,accumulatedspeed,"green")
xlabel("Position")
ylabel("Accumulated Speed")
title("Position / Accumulated Speed")







