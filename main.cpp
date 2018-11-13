#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <map>

template<typename T, T def_val, typename allocator = std::allocator<T>>
class Matrix{
    using index_pair = std::pair<std::size_t, std::size_t>;
    using map_storage = std::map< index_pair, T >;
    friend class Matrix;
    std::shared_ptr<map_storage> storage;
public:
    struct matrix_iterator{
        
        using map_iterator = typename std::map<index_pair, T>::iterator;
        matrix_iterator(const matrix_iterator& new_it){
            map_it = new_it.map_it;
        }
        matrix_iterator& operator=(const matrix_iterator& new_it){
            map_it = new_it.map_it;
            return this;
        }
        
        matrix_iterator& operator++(){
            map_it++;
            return *this;
        }

        friend bool operator==(const matrix_iterator& it1, const matrix_iterator& it2){
            return it1.map_it == it2.map_it; 
        }

        friend bool operator!=(const matrix_iterator& it1, const matrix_iterator& it2){
            return it1.map_it != it2.map_it;
        }

        std::tuple<int,int,T> operator*(){
            return std::make_tuple(map_it->first.first,map_it->first.second,map_it->second);
        }


        operator std::tuple<int,int,T>(){
            return std::make_tuple(map_it->first.first,map_it->first.second,map_it->second);
        }
        private:
        friend class Matrix;
        matrix_iterator(map_iterator it){map_it = it;}
        map_iterator  map_it;
    };

    using iterator = matrix_iterator; 
    using element_allocator_type = typename allocator::template rebind<iterator>::other;

    class row{
        friend class Matrix;
        std::size_t row_pos;
        std::shared_ptr<map_storage> storage;
        row(std::size_t r_pos, const std::shared_ptr<map_storage>& p_storage){
            row_pos=r_pos;
            storage = p_storage;
        }
        
        
        class cell{
            friend class row;
            std::shared_ptr<map_storage> storage; 
            std::size_t column_pos;
            std::size_t row_pos;
            cell(const row* owner, std::size_t c_pos){
                storage=owner->storage;
                row_pos=owner->row_pos;                
                column_pos = c_pos;
            }
            public:
            operator T(){
                if( storage->find( std::make_pair( row_pos, column_pos ) ) != storage->end() ){
                    return (*storage)[std::make_pair(row_pos,column_pos)];
                }else{
                    return def_val;
                }
            }
            T operator=(T new_val){
                if(new_val != def_val){
                    (*storage)[std::make_pair(row_pos,column_pos)]=new_val;
                }else{
                    auto element = storage->find( std::make_pair( row_pos, column_pos ) );
                    if( element != storage->end() ){
                        storage->erase(element);
                    }
                }
                return new_val;
            }
        };

        public:
            cell operator[](std::size_t py){
                return cell(this, py);
            }
    };

    iterator begin(){
        return matrix_iterator{storage->begin()};
    }

    iterator end(){
        return matrix_iterator{storage->end()};
    }

    const iterator cbegin(){ return const_cast<iterator const&>(this->begin()); }
    const iterator cend(){ return const_cast<iterator const&>(this->end()); }

    row operator[](std::size_t pos){
        return row(pos,storage);
    }

    std::size_t size(){
        return storage->size();
    }

    Matrix(){
        storage=std::make_shared<map_storage>();
    }

};


int main(){
    Matrix<int, 0> matrix; // бесконечная матрица int заполнена значениями -1
    for( int i = 0 ; i != 10 ; i++ ){
        for( int j = 0; j != 10 ; j++ ){
            if(i == j){
                matrix[i][j]=i;
            }
            if( i+j == 9 ){
                matrix[i][j]=j;
            }
        }
    }
    for(int i = 1 ; i != 9; i++){
        for(int j = 1; j != 9; j++){
            std::cout<<matrix[i][j];
            if(j != 8) std::cout<<' ';
        }
        std::cout<<'\n';
    }
    std::cout<<matrix.size()<<'\n';
    for(auto m: matrix){
        int x,y,v;
        std::tie(x,y,v) = m;
        std::cout<<x<<y<<v<<std::endl;
    }
    return 0;
}