function LT_decode(seed_num, sample_num, trial_num, use_NPF) 
    % science decoding
    % Copyright (c) 2021 by Jae-Won Kim, Jaeho Jeong, and Seong-Joon Park,
    % from Coding and Cryptography Lab (CCL), Department of Electrical and Computer Engineering,
    % Seoul National University, South Korea.
    % Email address: jaehoj@ccl.snu.ac.kr
    % Fixed by jiyeon park of Chonnam National University, Soutch Korea,
    % wldus8677@gmail.com
    % All Rights Reserved.

    % LT erasure decoding
    % input files: Reads which passed the error detection of RS code
    % output file: decoding_result.txt
    % image_restart.txt: used only for comparing the result

    %%%%%%%%%%%%%%%%%%%%parameters%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    tStart = tic;

    LT_L = 256; % number of bits in payload (128nt)
    LT_seed_bit = 4*8; % seed = index (16nt)
    RS_parity_num = 2; % RS parity 2byte (8nt)
    LT_K = 16050; % the number of message 
    LT_c = 0.025; % soliton distribution  
    LT_delta = 0.001; % soliton distribution 
    LT_s = LT_c * sqrt(LT_K) * log(LT_K/LT_delta); 

    RNG_input_seed = 10;
    soliton = zeros(1,LT_K);

    % solition init
    for i=1:LT_K
        if(i==1)
            soliton(i) = 1/LT_K; 
        end

        if(i~=1)
            soliton(i) = 1/i/(i-1); 
        end
    end

    tau = zeros(1,LT_K); 
    KSratio = round(LT_K / LT_s);

    % soliton tau init
    for i=1:LT_K 
        if(i == KSratio)
            tau(i) = LT_s * log(LT_s/LT_delta) / LT_K; 
        end

        if(i<KSratio)
            tau(i) = LT_s / LT_K / i;
        end
    end

    % LT_Z update
    LT_Z = 0;
    for i=1:LT_K 
        LT_Z = LT_Z + soliton(i) + tau(i);
    end

    % Soliton distribution for LT code
    Robust_soliton = zeros(1,LT_K); % 1 x 16050 zero
    for i=1:LT_K
        Robust_soliton(i) = (soliton(i) + tau(i)) / LT_Z;
    end

    sum_test = zeros(1,LT_K); 
    for i=1:LT_K
        if(i == 1) 
            sum_test(i) = Robust_soliton(1);
        else
            sum_test(i) = sum_test(i-1) + Robust_soliton(i);
        end
    end

    rng(RNG_input_seed); 
    Binary_Data_input = zeros(LT_K,LT_L); 

    % Load saved image bit file
    input_filename = sprintf('../../dataset/source/image_restart.txt'); 
    [FP] = fopen(input_filename,'r');
    input_data_save = fscanf(FP,'%d'); 
    fclose(FP);
    input_file_bit_size = size(input_data_save,1); 

    for i=1:LT_K 
        for j=1:LT_L 
            if((i-1)*LT_L+j<input_file_bit_size + 1)
                Binary_Data_input(i,j)=input_data_save((i-1)*LT_L+j);
            else
                Binary_Data_input(i,j)=randi(2)-1; % padding (0 or 1)
            end     
        end
    end

    %%%%%%%%%%%%%%%%%%%%%%%%For modification%%%%%%%%%%%%%%%%%%%%%%%%
    mode = '';
    if (use_NPF == 0)
        disp('use only PF');
        mode = 'PF';
    elseif (modes == 1)
        disp('use PF + NPF');
        mode = 'extraNPF';
    end
    disp(sample_num);

    Result_collect = zeros(1,1);  %% 0 - lack of inference, 1 - decoding success, 2 - decoding fail
    Trial_Number_collect = zeros(1,1);  % marking the number of trials
    LT_real_N_collect = zzeros(1,1);
    LT_decoding_observance_collect = zeros(1,1);
    inferred_idxs = zeros(1,1);
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    %%%%%%%%%%%%%%%%%LT parameter + seed parameter%%%%%%%%%%%%%%%%%%%
    % Load RS code passed data
    input_dataName = sprintf(".../../result/" + string(seed_num) + "/" + string(sample_num) + "/" + mode + "/RS_check/RSsuccess_" + string(trial_num) + ".txt"); 
    [FP1] = fopen(input_dataName,'r');      
    base_data_save = fscanf(FP1,'%s'); 
    fclose(FP1);

    raw_data_save = zeros(1,length(base_data_save) * 2);
    for i=1:length(base_data_save)
        if (base_data_save(i) == 'A')
            raw_data_save(i * 2 - 1) = 0;
            raw_data_save(i * 2) = 0;
        elseif (base_data_save(i) == 'C')
            raw_data_save(i * 2 - 1) = 0;
            raw_data_save(i * 2) = 1;
        elseif (base_data_save(i) == 'G')
            raw_data_save(i * 2 - 1) = 1;
            raw_data_save(i * 2) = 0;
        elseif (base_data_save(i) == 'T')
            raw_data_save(i * 2 - 1) = 1;
            raw_data_save(i * 2) = 1;
        end
    end

    number_of_strands = length(raw_data_save)/(LT_seed_bit+LT_L+8*RS_parity_num); 
    LT_parity_candidate = zeros(number_of_strands,LT_seed_bit+LT_L+8*RS_parity_num); 

    for i=1:number_of_strands 
        for j=1:(LT_seed_bit+LT_L+8*RS_parity_num) 
            LT_parity_candidate(i,j)=raw_data_save((i-1)*(LT_seed_bit+LT_L+8*RS_parity_num)+j);
        end
    end

    LT_decoding_observance = 0; %number of strands used for the decoding (count when RS ok but not inferred)
    LT_real_N = 0;         %during decoding, number of clusters saved for future inference (when degree one comes)

    Inferred_bitstream = zeros(LT_K,LT_L); 
    Inferred_idx = zeros(LT_K,1); 
    Current_bitstream = zeros(18000, LT_L);
    Current_xor_number = zeros(1, LT_L);
    Current_xor_state = zeros(18000, LT_K);
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    temp_vector_candidate = zeros(1,LT_seed_bit);

    trial_number = 0;  % line number in input file
    real_trial = 0;    % trial number

    while((real_trial < number_of_strands) && (sum(Inferred_idx)~=LT_K)) 
        flag=0; 
        if (trial_number < number_of_strands) 
            trial_number = trial_number + 1; 

            binary_candidate = zeros(1,LT_seed_bit+LT_L+8*RS_parity_num); 
            binary_candidate = LT_parity_candidate(trial_number,:);
            real_trial = real_trial+1;

            temp_vector_candidate = fliplr(binary_candidate(1:LT_seed_bit));

            Galois_register = bi2de(temp_vector_candidate);

            rng(Galois_register);
            degree_prop = rand;
            LT_generator_temp = zeros(LT_K,1);

            for j=1:LT_K %%16050
                if(j==1)
                    if(degree_prop < Robust_soliton(1))
                        symbolselection = datasample([1:LT_K],1,'Replace',false);
                        LT_generator_temp(symbolselection(1),1)=1;
                    end
                end

                if(j~=1)
                    if((sum_test(j-1)<=degree_prop) && (degree_prop < sum_test(j)))    
                        symbolselection = datasample([1:LT_K],j,'Replace',false);
                        for m=1:j
                            LT_generator_temp(symbolselection(m),1)=1;
                        end
                    end
                end

                if(degree_prop==1)
                    LT_generator_temp(:,1)=1;
                end
            end

            received_xor_state = find(LT_generator_temp);  %%%find index number of 1 in LT_generator_temp

            if(sum(Inferred_idx(received_xor_state)~=LT_generator_temp(received_xor_state)) ~= 0) %% has degree other than inferred ones
                LT_decoding_observance = LT_decoding_observance + 1;
                subtracted_sequence = binary_candidate(LT_seed_bit+1:LT_seed_bit+LT_L);

                for i=1:size(received_xor_state,1)
                    if(Inferred_idx(received_xor_state(i)) == 1)
                        subtracted_sequence = mod(subtracted_sequence + Inferred_bitstream(received_xor_state(i),:),2);
                        LT_generator_temp(received_xor_state(i)) = 0;
                    end              
                end

                test_degree_one = find(LT_generator_temp);
                test_test_degree_size = size(test_degree_one,1);


                if(test_test_degree_size==1)  %%% became degree 1 after inferring
                    %%%%below: we have new information from degree 1,
                    %%%%so we need a bp process

                    bp_update = 1;
                    while(bp_update == 1)
                        if(LT_real_N == 0)
                            bp_update = 0;
                            Inferred_bitstream(test_degree_one(1),:) = subtracted_sequence;
                            Inferred_idx(test_degree_one(1)) = 1;

                        end

                        if(LT_real_N ~= 0)    

                            Inferred_bitstream(test_degree_one(1),:) = subtracted_sequence;
                            Inferred_idx(test_degree_one(1)) = 1;


                            for i=1:LT_real_N
                                if((Current_xor_number(i) ~= 0) && (Current_xor_state(i,test_degree_one(1)) == 1))
                                    Current_bitstream(i,:) = mod(Current_bitstream(i,:) + subtracted_sequence,2);
                                    Current_xor_state(i,test_degree_one(1)) = 0;
                                    Current_xor_number(i) = Current_xor_number(i) - 1;
                                end
                            end                     
                            %%how to find the next one 
                            for i=1:LT_real_N
                                if(Current_xor_number(i) == 1)
                                    test_degree_one(1) = find(Current_xor_state(i,:));
                                    subtracted_sequence = Current_bitstream(i,:);
                                    bp_update = 1;
                                    break;                            
                                end

                                if(i==LT_real_N)
                                    bp_update = 0;
                                end
                            end
                        end                               
                    end

                else
                    LT_real_N = LT_real_N + 1;
                    Current_xor_state(LT_real_N,:) = transpose(LT_generator_temp);
                    Current_bitstream(LT_real_N,:) = subtracted_sequence;
                    Current_xor_number(LT_real_N) = test_test_degree_size;

                end

            end

            if(flag==1)
                trial_number = trial_number+1;
            end

        end
    end %while end

    Trial_Number_collect(1) = trial_number;  % save trial number
    LT_real_N_collect(1) = LT_real_N;
    LT_decoding_observance_collect(1) = LT_decoding_observance;

    if(sum(Inferred_idx)~=LT_K)
        fprintf('Final: Science_decoding_failure:inference_number_failure(0)\n')
        Result_collect(1) = 0;

    else
        if(Binary_Data_input == Inferred_bitstream)
            fprintf('Final: Science_decoding_success(1)\n')
            Result_collect(1) = 1;
        else
            fprintf('Final: Science_decoding_failure(2)\n')
            Result_collect(1) = 2;
        end   
    end

    output_filename = sprintf(".../../result/" + string(seed_num) + "/" + string(sample_num) + "/" + mode + "/LT_dec/result_" + string(trial_num) + ".txt");
    [FP] = fopen(output_filename,'wt');

    fprintf(FP,'%d %d %d %d %d %d\n', trial_num, Result_collect(1), Trial_Number_collect(1), LT_real_N_collect(1), LT_decoding_observance_collect(1), inferred_idxs(1));
    fclose(FP);

    tEnd = toc(tStart);
    disp(tEnd / 60);

    clear;
end