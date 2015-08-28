select * from (select id, id_poids from analyses where analyses.id_poids = 126);

select  analyses.id, analyses.id_poids from (select id, id_poids from analyses where analyses.id_poids = 126) as t1 left join analyses on t1.id = analyses.id+0; 

select t2.id_poids,tirages.* from (select  analyses.id, analyses.id_poids from (select id, id_poids from analyses where analyses.id_poids = 126) as t1 left join analyses on t1.id = analyses.id+1) as t2 left join tirages on t2.id=tirages.id where(t2.id_poids = 57);

select t2.id_poids,tirages.* from (select  analyses.id, analyses.id_poids from (select id, id_poids from analyses where analyses.id_poids = 126) as t1 left join analyses on t1.id = analyses.id+0) as t2 left join tirages on t2.id=tirages.id;