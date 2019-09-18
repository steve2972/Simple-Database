-- 1. Grass 타입의 포켓몬의 이름을 사전순으로 출력하세요
select name 
from Pokemon 
where type = 'Grass' 
order by name;

-- 2. Brown City나 Rainbow City 출신 트레이너의 이름을 사전순으로 출력하세요
select name 
from Trainer
where hometown = 'Brown City' or hometown ='Rainbow City' 
order by name;

-- 3. 모든 포켓몬의 type을 중복없이 사전순으로 출력하세요
select distinct type 
from Pokemon 
order by type;

-- 4. 도시의 이름이 B로 시작하는 모든 도시의 이름을 사전순으로 출력하세요
select name 
from city 
where name like 'b%' 
order by name;

--5.	이름이 M으로 시작하지 않는 트레이너의 고향을 사전순으로 출력하세요
select name from Trainer where name not like 'M%' order by name;

--6.	잡힌 포켓몬 중 가장 레벨이 높은 포켓몬의 별명을 사전순으로 출력하세요
select nickname from CatchedPokemon where level in (select max(level) from CatchedPokemon) order by nickname;

--7.	포켓몬의 이름이 알파벳 모음으로 시작하는 포켓몬의 이름을 사전순으로 출력하세요
select name from pokemon 
where name like 'A%' or name like 'E%' or name like 'I%' or name like 'O%' or name like 'U%' 
order by name;

--8.	잡힌 포켓몬의 평균 레벨을 출력하세요
select avg(level) from CatchedPokemon;

--9.	Yellow가 잡은 포켓몬의 최대 레벨을 출력하세요
select max(level) from CatchedPokemon where owner_id = 5;

--10.	트레이너의 고향 이름을 중복없이 사전순으로 출력하세요
select distinct hometown from Trainer order by hometown;

--11.	닉네임이 A로 시작하는 포켓몬을 잡은 트레이너의 이름과 포켓몬의 닉네임을 트레이너의 이름의 사전순으로 출력하세요
select nickname, name from CatchedPokemon join Trainer on owner_id = Trainer.id where nickname like 'A%' order by name;

--12.	Amazon 특성을 가진 도시의 리더의 트레이너 이름을 출력하세요
select Trainer.name from Gym join Trainer on leader_id = Trainer.id join City on Gym.city = City.name where City.description = 'Amazon';

--13.	불속성 포켓몬을 가장 많이 잡은 트레이너의 id와, 그 트레이너가 잡은 불속성 포켓몬의 수를 출력하세요
select owner_id, max(num) 
from (select owner_id, count(pid) as num
      from CatchedPokemon 
      join Pokemon on pid = Pokemon.id 
      where type = 'fire') as T

--14.	포켓몬 ID가 한 자리 수인 포켓몬의 type을 중복 없이 포켓몬 ID의 내림차순으로 출력하세요
select distinct type from Pokemon where id < 10 order by id desc;

--15.	포켓몬의 type이 Fire이 아닌 포켓몬의 수를 출력하세요
select count(*) from Pokemon where type != 'fire';

--16.	진화하면 id가 작아지는 포켓몬의 진화 전 이름을 사전순으로 출력하세요
select Pokemon.name from Evolution inner join Pokemon on before_id = Pokemon.id where before_id > after_id order by Pokemon.name;

--17.	트레이너에게 잡힌 모든 물속성 포켓몬의 평균 레벨을 출력하세요
select avg(level) from CatchedPokemon join Pokemon on pid = Pokemon.id where Pokemon.type = 'water';

--18.	체육관 리더가 잡은 모든 포켓몬 중 레벨이 가장 높은 포켓몬의 별명을 출력하세요
select nickname, level from (
  select owner_id, max(level) as m
  from CatchedPokemon join Gym on owner_id = leader_id 
  where owner_id 
  in (select leader_id from Gym) group by owner_id
  ) as T
inner join CatchedPokemon on CatchedPokemon.owner_id = T.owner_id
where CatchedPokemon.level = T.m

--19.	Blue city 출신 트레이너들 중 잡은 포켓몬들의 레벨의 평균이 가장 높은 트레이너의 이름을 사전순으로 출력하세요
select q1.tname from 
    (select owner_id, Trainer.name as tname, avg(level) as a 
    from CatchedPokemon join Trainer on owner_id = Trainer.id
    where Trainer.hometown = 'Blue City'
    group by owner_id
    order by a desc) as q1
join 
    (select max(a) as a from (select owner_id, Trainer.name as tname, avg(level) as a 
                        from CatchedPokemon join Trainer on owner_id = Trainer.id
                        where Trainer.hometown = 'Blue City'
                        group by owner_id
                        order by a desc) as T
     ) as q2
on q1.a = q2.a
order by q1.tname

--20.	같은 출신이 없는 트레이너들이 잡은 포켓몬중 진화가 가능하고 Electric 속성을 가진 포켓몬의 이름을 출력하세요
select Pokemon.name 
from Evolution 
inner join Pokemon on before_id = Pokemon.id 
where Pokemon.type = 'Electric' and Pokemon.id in
(
  select pid from CatchedPokemon
  join (select id, hometown, count(hometown) as n
        from Trainer 
        group by hometown) 
  as T
  on owner_id = T.id
  where T.n = 1
)

--21.	관장들의 이름과 각 관장들이 잡은 포켓몬들의 레벨 합을 레벨 합의 내림차 순으로 출력하세요
select gymT.tname, sum(level)
from CatchedPokemon 
join (select Trainer.name as tname, Trainer.id as tid 
      from Trainer join Gym on leader_id = Trainer.id) 
      as gymT on owner_id = gymT.tid
join Pokemon on pid = Pokemon.id
group by gymT.tname
order by sum(level) desc

--22.	가장 트레이너가 많은 고향의 이름을 출력하세요.
select hometown from (
  select id, hometown, count(hometown) as n 
  from Trainer group by hometown order by n desc
) as T
limit 1

--23.	Sangnok City 출신 트레이너와 Brown City 출신 트레이너가 공통으로 잡은 포켓몬의 이름을 중복을 제거하여 사전순으로 출력하세요
select distinct Pokemon.name
from CatchedPokemon 
join (select id, name 
      from Trainer 
      where hometown = 'Sangnok City' or hometown = 'Brown City') as T
on owner_id = T.id
join Pokemon on pid = Pokemon.id
order by Pokemon.name

--24.	이름이 P로 시작하는 포켓몬을 잡은 트레이너 중 상록 시티 출신인 트레이너의 이름을 사전순으로 모두 출력하세요
select Trainer.name from Trainer join
(select name, owner_id
from CatchedPokemon join Pokemon on pid = Pokemon.id
where name like 'P%') as T
on Trainer.id = T.owner_id
where Trainer.hometown = 'Sangnok City'

--25.	트레이너의 이름과 그 트레이너가 잡은 포켓몬의 이름을 출력하세요. 트레이너 이름의 사전 순으로 정렬하고, 각 트레이너가 잡은 포켓몬 간에도 사전 순으로 정렬하세요.
select Trainer.name, Pokemon.name
from CatchedPokemon join Trainer on owner_id = Trainer.id
join Pokemon on pid = Pokemon.id
order by Trainer.name, Pokemon.name

--26.	2단계 진화만 가능한 포켓몬의 이름을 사전순으로 출력하세요
select Pokemon.name
from Evolution e1 join Evolution e2
on e1.before_id = e2.before_id
join Pokemon on e1.before_id = Pokemon.id
where Pokemon.name not in 
  (select Pokemon.name
  from Evolution e1 join Evolution e2
  on e1.after_id = e2.before_id
  join Pokemon on e1.before_id = Pokemon.id)
and Pokemon.name not in 
  (select Pokemon.name
  from Evolution e1 join Evolution e2
  on e1.after_id = e2.before_id
  join Pokemon on e1.after_id = Pokemon.id)

--27.	상록 시티의 관장이 잡은 포켓몬들 중 포켓몬의 타입이 WATER 인 포켓몬의 별명을 사전순으로 출력하세요
select CatchedPokemon.nickname
from CatchedPokemon join (
  select Trainer.id as tid
  from Trainer join Gym on leader_id = Trainer.id 
  where Gym.city = 'Sangnok City') as T
on owner_id = T.tid
join Pokemon on pid = Pokemon.id
where Pokemon.type = 'Water'
order by CatchedPokemon.nickname

--28.	트레이너들이 잡은 포켓몬 중 진화한 포켓몬이 3마리 이상인 경우 해당 트레이너의 이름을 사전순으로 출력하세요
select Trainer.name, count(pid) as num 
from CatchedPokemon join Trainer on owner_id = Trainer.id
where pid in 
(select Pokemon.id from Pokemon join Evolution on Pokemon.id = after_id)
group by owner_id
having num >= 3 
order by Trainer.name

--29.	어느 트레이너에게도 잡히지 않은 포켓몬의 이름을 사전 순으로 출력하세요
select Pokemon.name from Pokemon where Pokemon.id not in
(select distinct pid from CatchedPokemon)
order by Pokemon.name

--30.	각 출신 도시별로 트레이너가 잡은 포켓몬중 가장 레벨이 높은 포켓몬의 레벨을 내림차 순으로 출력하세요.
select max(level) a
from CatchedPokemon 
join Trainer on owner_id = Trainer.id
group by Trainer.hometown
order by a desc

--31.	포켓몬 중 3단 진화가 가능한 포켓몬의 ID 와 해당 포켓몬의이름을 1단진화 형태 포켓몬의이름, 2단진화 형태 포켓몬의 이름, 3단 진화 형태 포켓몬의 이름을 ID의 오름차순으로 출력하세요
select p1.id, p1.name, p2.name, p3.name
from Evolution e1
join Evolution e2 on e1.after_id = e2.before_id
join Pokemon as p1 on e1.before_id = p1.id
join Pokemon as p2 on e2.before_id = p2.id
join Pokemon as p3 on e2.after_id = p3.id
order by p1.id
